#include "LinuxEvents.h"

class XnLinuxPosixEvent : public XnLinuxEvent
{
public:
	XnLinuxPosixEvent(XnBool bManualReset);

	virtual XnStatus Init();
	virtual XnStatus Destroy();
	virtual XnStatus Set();
	virtual XnStatus Reset();
	virtual XnStatus Wait(XnUInt32 nMilliseconds);

private:
	pthread_cond_t m_cond;
	pthread_mutex_t m_mutex;
};

