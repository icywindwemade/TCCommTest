#include "NetworkMonitor.h"
#include "NetworkManager.h"

pthread_t MonThread = 0;
pthread_mutex_t MonLock;
pthread_cond_t MonCond = PTHREAD_COND_INITIALIZER;


void* MonitorThreadProc( void* InParam )
{
	( (NNetworkMonitor*)InParam )->Run();
    
#ifdef NET_DEVICE_WIN
	return 0;
#else //  NET_DEVICE_IOS
	printf("pthread_exit()");
	pthread_exit((void *)NULL);
#endif // NET_DEVICE_WIN
    
}

NNetworkMonitor * NNetworkMonitor::Instance() {
    static NNetworkMonitor monitor;
    return &monitor;
}

void NNetworkMonitor::Run() {
    useconds_t nsinv = (int)1000000*(1.0/m_FrameTime); // nanosecond sleep
    while (isRunning) {
        //printf("Montior is running\n");
        usleep(nsinv);
        NNetworkManager::Instance()->Update( m_FrameTime );
    }
    printf("Montior stopped running\n");
}

bool NNetworkMonitor::Start() {
   	int Result = pthread_create( &MonThread, 0, MonitorThreadProc, this );
	if( Result != 0 )
    {
        isRunning = false;
    }

    isRunning = true;
    return isRunning;
}

void NNetworkMonitor::Stop() {
    isRunning = false;
}