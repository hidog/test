/************************************************************************************************************************************
	
	色標檢測放在這邊

/*************************************************************************************************************************************/

#ifndef _COLOR_CODE_H
#define _COLOR_CODE_H

#include "stdafx.h"
#include "../BLOB/blob.h"
#include "../BLOB/watershed.h"
#include "channel.h"



#ifdef _OPENPIV_
#include "../src/andreaengine.h"
#include "../src/settings.h"
#include "../src/progress.h"
#include "../src/datacontainer.h"
#endif

#ifdef _OPENPIV_
class	ColorCode : public AndreaEngine
#else
class	ColorCode
#endif
{

	private:
		QImage			image;
		QImage			label;								// 用watershed label 將色標切開來了
		QImage			graytone;							// 做gray tone


#ifdef _OPENPIV_
		DataContainer	*filedata;
		Settings		*settings;
		QMutex			mutex;
		Progress		*progress;							// 負責回報進度用的
		
	signals:
		void			debugSignal( QString );				// 除錯訊息
#endif

	protected:
	public:

#ifdef _OPENPIV_
		ColorCode( Settings *settingsPass, DataContainer *filedataPass );		// 建構
		void		dosomething();												// 都利用這個funtion來做事情. 取代原本的main
#else
		ColorCode();															// 建構 無傳入值
#endif

		virtual		~ColorCode(); 												// 解構


};




#endif