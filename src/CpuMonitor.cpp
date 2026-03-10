#include "CpuMonitor.h"

CpuMonitor::CpuMonitor()
{
	running.store(true, std::memory_order_relaxed);
    query = {};
    counter = {};
    counterVal = {};
    //GetCpuTemperature(&cpuTemp);
}

void CpuMonitor::pollCPUMetrics()
{
    //running.store(true, std::memory_order_relaxed); // might not need this here

    if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
    {
        std::cout << "Error line 15 CpuMonitor.cpp\n";
        running.store(false);
        return;
    }
    if (PdhAddCounterW(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
    {
        std::cout << "Error line 22 CpuMonitor.cpp\n";
        running.store(false);
        PdhCloseQuery(query);
        return;
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const DWORD coreCount = sysInfo.dwNumberOfProcessors;
    coreCounters.clear();
    coreCounters.reserve(coreCount);
    coreHistories.clear();
    coreHistories.assign(coreCount, {});  // coreCount histories

    for (DWORD i = 0; i < coreCount; ++i)
    {
        std::wstring buffer = L"\\Processor(" + std::to_wstring(i) + L")\\% Processor Time";

        PDH_HCOUNTER c{};
        if (PdhAddCounterW(query, buffer.c_str(), 0, &c) != ERROR_SUCCESS)
        {
            std::cout << "Error line 33 CpuMonitor.cpp\n";
            running.store(false);
            PdhCloseQuery(query);
            return;
        }
        coreCounters.push_back(c);
    }

    std::cout << "Number of cores added to vector: " << coreCounters.size() << '\n';

    cpuThread = std::thread([this, coreCount]()
        {
            PdhCollectQueryData(query);
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));

            while (running.load(std::memory_order_relaxed))
            {
                if (PdhCollectQueryData(query) != ERROR_SUCCESS)
                    continue;

                PDH_FMT_COUNTERVALUE v{};

                // total
                if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &v) == ERROR_SUCCESS)
                {
                    cpuValue.store(static_cast<float>(v.doubleValue));
                    cpuHistory[index] = static_cast<float>(v.doubleValue);
                }

                // per core
                for (size_t core = 0; core < coreCount; ++core)
                {
                    if (PdhGetFormattedCounterValue(coreCounters[core], PDH_FMT_DOUBLE, nullptr, &v) == ERROR_SUCCESS)
                        coreHistories[core][index] = static_cast<float>(v.doubleValue);
                }

                index = (index + 1) % cpuHistory.size();
                std::this_thread::sleep_for(std::chrono::milliseconds(450));
            }
        });

    SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
}
void CpuMonitor::stopPolling()
{
    if (!running.exchange(false))
        return;

    if (cpuThread.joinable())
        cpuThread.join();

    if (query != nullptr)
    {
        PdhCloseQuery(query);
        query = nullptr;
        counter = nullptr;
    }
}

HRESULT CpuMonitor::SetCpuTemperature(LPLONG pTemperature)
{
    if (pTemperature == NULL)
        return E_INVALIDARG;

    *pTemperature = -1;
    HRESULT ci = CoInitialize(NULL);
    HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (SUCCEEDED(hr))
    {
        IWbemLocator* pLocator;
        hr = CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLocator);
        if (SUCCEEDED(hr))
        {
            IWbemServices* pServices;
            BSTR ns = SysAllocString(L"root\\WMI");
            hr = pLocator->ConnectServer(ns, NULL, NULL, NULL, 0, NULL, NULL, &pServices);
            pLocator->Release();
            SysFreeString(ns);
            if (SUCCEEDED(hr))
            {
                BSTR query = SysAllocString(L"SELECT * FROM MSAcpi_ThermalZoneTemperature");
                BSTR wql = SysAllocString(L"WQL");
                IEnumWbemClassObject* pEnum;
                hr = pServices->ExecQuery(wql, query, WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);
                SysFreeString(wql);
                SysFreeString(query);
                pServices->Release();
                if (SUCCEEDED(hr))
                {
                    IWbemClassObject* pObject;
                    ULONG returned;
                    hr = pEnum->Next(WBEM_INFINITE, 1, &pObject, &returned);
                    pEnum->Release();
                    if (SUCCEEDED(hr))
                    {
                        BSTR temp = SysAllocString(L"CurrentTemperature");
                        VARIANT v;
                        VariantInit(&v);
                        hr = pObject->Get(temp, 0, &v, NULL, NULL);
                        pObject->Release();
                        SysFreeString(temp);
                        if (SUCCEEDED(hr))
                            *pTemperature = V_I4(&v);

                        VariantClear(&v);
                    }
                }
            }
            if (ci == S_OK)
                CoUninitialize();
        }
    }
    return hr;
}

const std::string CpuMonitor::getCPUStr() const
{
    char CPUBrandString[64] = "";

    int CPUInfo[4] = {};
    unsigned nExIds, i = 0;
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        switch (i)
        {
        case 0x80000002:
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            break;
        case 0x80000003:
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            break;
        case 0x80000004:
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
            break;
        }
    }
    return std::string(CPUBrandString);
}

const std::atomic<float>& CpuMonitor::getCPUValue() const
{
    return cpuValue;
}

const std::array<float, 10>& CpuMonitor::getCPUMetrics() const
{
	return cpuHistory;
}

const std::vector<std::array<float, 10>>& CpuMonitor::getIndividualCoreHistories() const
{
    return coreHistories;
}

CpuMonitor::~CpuMonitor()
{
    stopPolling();
}
