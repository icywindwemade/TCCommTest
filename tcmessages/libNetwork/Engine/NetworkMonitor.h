#ifndef NETWORK_MONITOR_H
#define NETWORK_MONTIOR_H

class NNetworkMonitor {
    bool isRunning;
    int m_FrameTime;
    
public:
    NNetworkMonitor(): isRunning(false), m_FrameTime(33)
    {}
    
    ~NNetworkMonitor()
    {}
    
    void Run();
    bool Start();
    void Stop();
    static NNetworkMonitor * Instance();  // Singleton instance
};

#endif