#ifndef ANDREAENGINE_H
#define ANDREAENGINE_H

/***************************************************************************************************************************************************************
	原型 
	所有的Engine都繼承他
/****************************************************************************************************************************************************************/


#include <QMutex>
#include "settings.h"
#include "datacontainer.h"




class AndreaEngine
{
    public:
		// 建構
        AndreaEngine( );
		
        virtual			~AndreaEngine();

       // PivData			*operator() ( MetaData dataPass );

		virtual	void	dosomething()	=	0;			// 所有支援的物件都必須用這個來執行主要的動作

    protected:

    private:
        QMutex	mutex;

};

#endif // ANDREAENGINE_H
