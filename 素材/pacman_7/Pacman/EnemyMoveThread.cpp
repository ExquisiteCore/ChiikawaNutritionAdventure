#include "EnemyMoveThread.h"

EnemyMoveThread::EnemyMoveThread(QObject* parent):QThread(parent),Timer(this),End(false)
{
}

void EnemyMoveThread::run()
{
	while(!End)
	{
		QTimer::singleShot(0, this, &EnemyMoveThread::MoveSignal);
		msleep(10);
	}
}

void EnemyMoveThread::EndSignal(void)
{
	End = true;
}
