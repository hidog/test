#include "pivthread.h"

#include <QVector>
#include <QSemaphore>
#include <iostream>




PivThread::PivThread( int _id, QSemaphore *freePass, QSemaphore *usedPass, QMutex *mutexPass, /*QList<int> listPass,*/ QObject *parent ) : QThread(parent), id(_id)
{
	free				=	freePass;
    used				=	usedPass;
    mutex				=	mutexPass;
//    list				=	listPass;

    analysisCreated		=	false;
    settingsSet			=	false;
    filedataSet			=	false;
    filter				=	false;

    abort				=	false;
}


PivThread::~PivThread()
{
    wait();
    switch(_processor)
    {
		default:
		//	delete	engine;
			break;
    }

}

void PivThread::setSettings(Settings *settingsPass)
{
    settings	=	settingsPass;
    _processor	=	settings->processor();
    settingsSet	=	true;
}


void PivThread::setFileData(DataContainer *filedataPass)
{

	QString		str	=	"setFiledata ";
	str	+=	'0'+list.size();
	progress->debuger( str );


    filedata = filedataPass;
	
    filedataSet = true;
}


void PivThread::initializeProcessor()
{
    if (filedataSet && settingsSet)
    {
		switch(_processor)
		{
			case	OpenPIV::SubPixBlob:
				engine		=	new SUBPIX_BLOB( settings, filedata );
				break;

			case	OpenPIV::Watershed:
				engine		=	new Watershed( settings, filedata );
				break;

			case	OpenPIV::FFT:
				engine		=	new FFT( settings, filedata );
				break;

			case	OpenPIV::Steer:
				engine		=	new Steer( settings, filedata );
				break;

			case	OpenPIV::Edge:
				engine		=	new Edge( settings, filedata );
				break;

			case	OpenPIV::Corner:
				engine		=	new Corner( settings, filedata );
				break;

			case	OpenPIV::Channel:
				//engine		=	new Channel( settings, filedata );
				break;

			case	OpenPIV::ColorCode:
				engine		=	new ColorCode( settings, filedata );
				break;

			default:
				break;
        }
    }
}



void PivThread::stopProcess()
{
    abort = true;
}




emboss::EMBOSS_DARK_LIGHT	PivThread::int2dark( int a )
{
	emboss::EMBOSS_DARK_LIGHT	rdata;
	
	switch(a)
	{
		case 0 :
			rdata	=	emboss::EMBOSS_DARK;
			break;
		case 1 :
			rdata	=	emboss::EMBOSS_LIGHT;
			break;
		default:
			rdata	=	emboss::EMBOSS_DARK_LIGHT_MAX;
	}

	return	rdata;
}






emboss::EMBOSS_DIRECTION	PivThread::int2direction( int a )
{
	emboss::EMBOSS_DIRECTION	rdata;

	switch(a)
	{
		case 0 :
			rdata	=	emboss::EMBOSS_UP;
			break;
		case 1:
			rdata	=	emboss::EMBOSS_DOWN;
			break;
		case 2:
			rdata	=	emboss::EMBOSS_LEFT;
			break;
		case 3:
			rdata	=	emboss::EMBOSS_RIGHT;
			break;
		case 4:
			rdata	=	emboss::EMBOSS_LEFT_UP;
			break;
		case 5:
			rdata	=	emboss::EMBOSS_LEFT_DOWN;
			break;
		case 6:
			rdata	=	emboss::EMBOSS_RIGHT_UP;
			break;
		case 7:
			rdata	=	emboss::EMBOSS_RIGHT_DOWN;
			break;
		case 8 :
			rdata	=	emboss::EMBOSS_ALL_DIRECTION;
			break;
		case 9:
			rdata	=	emboss::EMBOSS_UP_DOWN;
			break;
		case 10:
			rdata	=	emboss::EMBOSS_LEFT_RIGHT;
			break;

		case 11:
			rdata	=	emboss::EMBOSS_LU_RD;
			break;

		case 12:
			rdata	=	emboss::EMBOSS_LD_RU;
			break;

		default:
			rdata	=	emboss::EMBOSS_DIRECTION_MAX;


	}

	return	rdata;
}









// 之後需要再改成其他資料結構
typedef	QVector<emboss::SquareData>	EmbossData;




// 這邊先關掉loop功能
void PivThread::run2()
{
	progress->debuger( QString("PivThread::run") );

    mutex->lock();

	int		fsize	=	settings->file_size;
	char	str[100];

	int		i,	j,	k;
	int		x,	y;

	// 底下是 emboss 操作
	emboss::Emboss	em;

	// 存放資料用
	EmbossData					data;

	// 各項設定
	emboss::ShadowSettings		emboss_settings;


	// 第一筆資料
	/*emboss_settings.x			=	337;
	emboss_settings.y			=	244;
	emboss_settings.out_x		=	397;
	emboss_settings.out_y		=	592;
	emboss_settings.size		=	16;
	emboss_settings.threshold	=	10;
	emboss_settings.direction	=	emboss::EMBOSS_UP;
	emboss_settings.enhance		=	false;
	emboss_settings.dark_light	=	emboss::EMBOSS_DARK;
	data.append( emboss::SquareData(emboss_settings) );*/
	

	// 將資料塞入檢測區
	QVector<PIXEL>	p_vector	=	filedata->getPixelList();
	PIXEL			p_data;
	for( i = 0; i < p_vector.size(); i++ )
	{
		p_data	=	p_vector[i];

		emboss_settings.x			=	p_data.x;
		emboss_settings.y			=	p_data.y;
		emboss_settings.size		=	p_data.square_size;
		emboss_settings.threshold	=	settings->shadow_depth_threshold;
		emboss_settings.direction	=	int2direction(settings->shadow_dir); //int2direction( p_data.direction );
		emboss_settings.enhance		=	settings->inhence_mode;
		emboss_settings.dark_light	=	int2dark( settings->dark_light );   //int2dark( emboss::EMBOSS_DARK );

		data.append( emboss::SquareData(emboss_settings) );
	}
	QImage	image;


//for( int id = 0; id <= 180; id++ )
{
//	char	str[100];
//	sprintf(str,"D:\\ROT_1\\%06d.bmp",id);
	
	//image	=	QImage(str);
	image	=	filedata->get_image();

//	em.execution( image );

	int kkkk =  data[0].shadow.size1();


	//for( i = 0; i < data.size(); i++ )
	//	em.mark_on_image( image );

//	sprintf(str, "D:\\output\\%06d.bmp",id);
//	image.save(str);

	progress->outputImage( 0, image );


}	





    mutex->unlock();

	progress->debuger( QString("finish!!"), QColor(255,0,0) );


	emit(finished(id));
}


/*====================================================================================================
	設定Progress
/*====================================================================================================*/
void	PivThread::setProgress( Progress *passGress )
{
	progress	=	passGress;
}



/*====================================================================================================
	PivThread實際執行process的地方
/*====================================================================================================*/
int PivThread::process()
{
	QString		str	=	"PivThread::process";
	progress->debuger(str);

    mutex->lock();
    initializeProcessor();
    mutex->unlock();

    start();
	return 0;
}






// 這邊先關掉loop功能
void PivThread::run()
{
	progress->debuger( QString("PivThread::run") );

    mutex->lock();

	char	str[100];

	int		i,	j,	k;
	int		x,	y;

	// 底下是 emboss 操作
	emboss::Emboss	em;

	// 存放資料用
	EmbossData					data;

	// 各項設定
	emboss::ShadowSettings		emboss_settings;

	QImage	image;

	


	// 1
	emboss_settings.x				=	823;
	emboss_settings.y				=	268;
	emboss_settings.size			=	13;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_UP;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	6;
	emboss_settings.em_settings.up_line_2		=	6;
	emboss_settings.em_settings.down_line_1		=	8;
	emboss_settings.em_settings.down_line_2		=	8;

	data.append( emboss::SquareData(emboss_settings) );

	// 2
	emboss_settings.x				=	815;
	emboss_settings.y				=	280;
	emboss_settings.size			=	10;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_DOWN;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	4;
	emboss_settings.em_settings.up_line_2		=	4;
	emboss_settings.em_settings.down_line_1		=	6;
	emboss_settings.em_settings.down_line_2		=	6;

	data.append( emboss::SquareData(emboss_settings) );

	// 3
	emboss_settings.x				=	734;
	emboss_settings.y				=	275;
	emboss_settings.size			=	9;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_LEFT;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	3;
	emboss_settings.em_settings.up_line_2		=	3;
	emboss_settings.em_settings.down_line_1		=	5;
	emboss_settings.em_settings.down_line_2		=	5;

	data.append( emboss::SquareData(emboss_settings) );


	// 4
	emboss_settings.x				=	854;
	emboss_settings.y				=	276;
	emboss_settings.size			=	9;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_RIGHT;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	2;
	emboss_settings.em_settings.up_line_2		=	2;
	emboss_settings.em_settings.down_line_1		=	4;
	emboss_settings.em_settings.down_line_2		=	4;

	data.append( emboss::SquareData(emboss_settings) );

	// 5
	emboss_settings.x				=	768;
	emboss_settings.y				=	367;
	emboss_settings.size			=	7;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_LEFT_UP;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	1;
	emboss_settings.em_settings.up_line_2		=	3;
	emboss_settings.em_settings.down_line_1		=	3;
	emboss_settings.em_settings.down_line_2		=	6;

	data.append( emboss::SquareData(emboss_settings) );

	// 6
	emboss_settings.x				=	814;
	emboss_settings.y				=	477;
	emboss_settings.size			=	8;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_LEFT_DOWN;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	1;
	emboss_settings.em_settings.up_line_2		=	2;
	emboss_settings.em_settings.down_line_1		=	6;
	emboss_settings.em_settings.down_line_2		=	6;

	data.append( emboss::SquareData(emboss_settings) );

	// 7
	emboss_settings.x				=	815;
	emboss_settings.y				=	393;
	emboss_settings.size			=	6;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_RIGHT_UP;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	1;
	emboss_settings.em_settings.up_line_2		=	1;
	emboss_settings.em_settings.down_line_1		=	3;
	emboss_settings.em_settings.down_line_2		=	4;

	data.append( emboss::SquareData(emboss_settings) );

	// 8
	emboss_settings.x				=	795;
	emboss_settings.y				=	367;
	emboss_settings.size			=	8;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_RIGHT_DOWN;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	emboss_settings.em_settings.up_line_1		=	1;
	emboss_settings.em_settings.up_line_2		=	2;
	emboss_settings.em_settings.down_line_1		=	3;
	emboss_settings.em_settings.down_line_2		=	4;

	data.append( emboss::SquareData(emboss_settings) );


	// 9
	emboss_settings.x				=	765;
	emboss_settings.y				=	268;
	emboss_settings.size			=	20;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_ALL_DIRECTION;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	data.append( emboss::SquareData(emboss_settings) );

	// 10
	emboss_settings.x				=	765;
	emboss_settings.y				=	268;
	emboss_settings.size			=	27;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_ALL_DIRECTION;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	data.append( emboss::SquareData(emboss_settings) );

	// 11
	emboss_settings.x				=	765;
	emboss_settings.y				=	268;
	emboss_settings.size			=	11;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_ALL_DIRECTION;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	data.append( emboss::SquareData(emboss_settings) );

	// 12
	emboss_settings.x				=	765;
	emboss_settings.y				=	268;
	emboss_settings.size			=	7;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_ALL_DIRECTION;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	data.append( emboss::SquareData(emboss_settings) );

	// 13
	emboss_settings.x				=	765;
	emboss_settings.y				=	268;
	emboss_settings.size			=	25;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_ALL_DIRECTION;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	data.append( emboss::SquareData(emboss_settings) );

	// 14
	emboss_settings.x				=	765;
	emboss_settings.y				=	268;
	emboss_settings.size			=	18;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_ALL_DIRECTION;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	data.append( emboss::SquareData(emboss_settings) );


	// 15
	/*emboss_settings.x				=	1740;
	emboss_settings.y				=	456;
	emboss_settings.size			=	48;
	emboss_settings.threshold		=	10;
	emboss_settings.direction		=	emboss::EMBOSS_UP_DOWN;
	emboss_settings.enhance			=	false;
	emboss_settings.dark_light		=	emboss::EMBOSS_DARK;

	data.append( emboss::SquareData(emboss_settings) );*/



	// 產生允許區
	for( i = 0; i < data.size(); i++ )
		em.generate_allow_area();



	// 開始進行檢測
for( int id = 0; id <= 91; id++ )
{
	//cout << "The " << id << " pic...\n";

	sprintf(str, "D:\\red_river\\%06d.bmp", id );
	image	=	QImage(str);




//	em.execution( image );

	for( i = 0; i < data.size(); i++ )
	{
		
		//em.mark_on_image_mid_area( data[i], image );
//		em.result_output_emboss( image );
		//em.emboss_result( image, data[i] );
		//cout << "roi " << i << " ... out number = " << data[i].em_data.out_shadow << endl;
	}
	//cout << endl;


	sprintf(str, "D:\\output\\%06d.bmp",id);
	image.save(str);


}






    mutex->unlock();

	progress->debuger( QString("finish!!"), QColor(255,0,0) );


	emit(finished(id));
}
