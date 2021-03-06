/* get_sub_terafly_plugin.cpp
 * This is a test plugin, you can use it as a demo.
 * 2018-6-12 : by Yongzhang
 */
 
#include "v3d_message.h"
#include <vector>
#include <iostream>
#include "basic_surf_objs.h"
#include "get_sub_terafly_plugin.h"
#include "algorithm"
#include <v3d_interface.h>
#include <QtGui>
#include <string>
#include <stdio.h>
using namespace std;
Q_EXPORT_PLUGIN2(get_sub_terafly, GETBLOCK);
struct TRACE_LS_PARA
{
    int is_gsdt;
    int is_break_accept;
    int  bkg_thresh;
    double length_thresh;
    int  cnn_type;
    int  channel;
    double SR_ratio;
    int  b_256cube;
    int b_RadiusFrom2D;
    int block_size;
    int adap_win;
    int tracing_3D;
    int tracing_comb;
    int grid_trace;
    int global_name;
    int soma;

    V3DLONG in_sz[3];

    int  visible_thresh;//for APP1 use only

    int  seed_win; //for MOST use only
    int  slip_win; //for MOST use only


    Image4DSimple* image;
    LandmarkList listLandmarks;
    QString tcfilename,inimg_file,rawfilename,markerfilename,swcfilename,inimg_file_2nd,inimg_file1,inimg_file2,swc_file2;
};


bool get_sub_terafly(V3DPluginCallback2 &callback,QWidget *parent,QString outimg_file);
 
QStringList GETBLOCK::menulist() const
{
	return QStringList() 
        <<tr("get_block")
		<<tr("about");
}

QStringList GETBLOCK::funclist() const
{
	return QStringList()
        <<tr("get_block")
		<<tr("help");
}

void GETBLOCK::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("get_block"))
	{
        QString outimg_file;
        get_sub_terafly(callback,parent,outimg_file);

	}
	else
	{
		v3d_msg(tr("This is a test plugin, you can use it as a demo.. "
            "Developed by OYQ, 2018-11-20"));
	}
}

bool GETBLOCK::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
    TRACE_LS_PARA P;
    vector<char*> * pinfiles = (input.size() >= 1) ? (vector<char*> *) input[0].p : 0;
    vector<char*> * image = (input.size() >= 2) ? (vector<char*> *) input[1].p : 0;
    vector<char*> infiles = (pinfiles != 0) ? * pinfiles : vector<char*>();
    vector<char*> paras = (image != 0) ? * image : vector<char*>();

    vector<char*> * poutfiles = (output.size() >= 1) ? (vector<char*> *) output[0].p : 0;
    vector<char*> * outimage = (output.size() >= 2) ? (vector<char*> *) output[1].p : 0;
    vector<char*> outlist = (poutfiles != 0) ? * poutfiles : vector<char*>();
    //vector<char*> parasimg = (outimage != 0) ? * outimage : vector<char*>();
    //vector<char*>* outlist = NULL;

    if (func_name == tr("get_block"))
	{

        if(infiles.empty())
        {
            cerr<<"Need input two image and one swc files"<<endl;
            return false;
        }

        P.swc_file2 = infiles[0];
        P.inimg_file1 =paras[0];
        P.inimg_file2=paras[1];
        P.image = 0;

        QString swc_folder_path;
        QString image1_folder_path;
        QString image2_folder_path;
        QString fileSaveFolder1,fileSaveFolder2;
        swc_folder_path=P.swc_file2;
        image1_folder_path=P.inimg_file1;
        image2_folder_path=P.inimg_file2;
        //fileSaveFolder2=parasimg[0];

        if (output.size()==1)
        {
            //outlist = (vector<char*>*)(output.at(0).p);
            fileSaveFolder1= outlist[0];
        }
        else {printf("You must specify one result folder.\n");return false; }


        // read swc files from a folder
        QStringList swcList = importFileList_addnumbersort(QString(swc_folder_path));
        vector<NeuronTree> nt_list;


        for(V3DLONG i = 0; i < swcList.size(); i++)
        {

            QString curPathSWC = swcList.at(i);
            NeuronTree temp = readSWC_file(curPathSWC);
            QList<NeuronSWC> currswc=temp.listNeuron;
            int totall=currswc.size();
            int maxnumN=currswc.at(totall-1).n;
            cout<<"max number:\t"<<maxnumN<<endl;

            QList<CellAPO> file_inmarkers;
            int arr[10]={ 0 };
            srand((unsigned)time(NULL));          
            for(int i=0;i<10;i++)
            {
              arr[i]=rand() % maxnumN+0;
              CellAPO t;
                  t.x = currswc.at(arr[i]).x;
                  t.y = currswc.at(arr[i]).y;
                  t.z = currswc.at(arr[i]).z;
                  t.volsize = 50;
                  t.color.r = 0;
                  t.color.g = 0;
                  t.color.b = 255;
                  file_inmarkers.push_back(t);

              V3DLONG im_cropped_sz[4];
              unsigned char * im_cropped = 0;
              unsigned char * im_cropped2 = 0;
              double xe=currswc.at(arr[i]).x;
              double ye=currswc.at(arr[i]).y;
              double ze=currswc.at(arr[i]).z;
              double l_x = 128;
              double l_y = 128;
              double l_z = 64;

              V3DLONG xb = xe-l_x;
              V3DLONG xn = xe+l_x-1;
              V3DLONG yb = ye-l_y;
              V3DLONG yn = ye+l_y-1;
              V3DLONG zb = ze-l_z;
              V3DLONG zn = ze+l_z-1;

              V3DLONG pagesz;
              pagesz = (xn-xb+1)*(yn-yb+1)*(zn-zb+1);
              im_cropped_sz[0] = xn-xb+1;
              im_cropped_sz[1] = yn-yb+1;
              im_cropped_sz[2] = zn-zb+1;
              im_cropped_sz[3] = 1;


              try {im_cropped = new unsigned char [pagesz];}
              catch(...)  {cout<<"cannot allocate memory for image_mip."<<endl; return false;}
              try {im_cropped2 = new unsigned char [pagesz];}
              catch(...)  {cout<<"cannot allocate memory for image_mip."<<endl; return false;}

              //****************the first image file************************//
              im_cropped = callback.getSubVolumeTeraFly(image1_folder_path.toStdString(),xb,xn+1,yb,yn+1,zb,zn+1);



              V3DLONG ccou=0;
              for (int i=0;i<sizeof(im_cropped);i++)
              {
                  int numof_iden=(int)(im_cropped[i]);
                  if (numof_iden==0) ccou+=1;continue;
              }
              if (ccou==pagesz)
              {
                  v3d_msg(QString("[%1] file have no values!").arg(QString (image1_folder_path)));
                 /* QString outfileLabel = "NO_IDN";
                  // check report
                  FILE * fp=0;
                  fp = fopen((char *)qPrintable(outfileLabel+QString(".txt")), "wt");
                  // check.1
                  QString writename=curPathSWC;
                  fprintf(fp, "images to be double checked \t%s\n",writename);
                  fclose(fp);
                  */
              }
              QString outimg_file;
              QString numofrandom=QString("%1").arg(arr[i]);
              QString outimg_file1= "random_check.tif";
              //cout<<"im_cropped:\t"<<im_cropped[0]<<im_cropped[100]<<im_cropped[2]<<im_cropped[31]<<endl;

              outimg_file=fileSaveFolder1+"/"+"J."+QString(numofrandom)+"."+QString(outimg_file1);
//            cout<<"numofrandom:\t"<<numofrandom.toStdString().c_str()<<endl;
//            cout<<"outimg_file1:\t"<<outimg_file1.toStdString().c_str()<<endl;
//            cout<<"outimg_file:\t"<<outimg_file.toStdString().c_str()<<endl;
              simple_saveimage_wrapper(callback, outimg_file.toStdString().c_str(),(unsigned char *)im_cropped,im_cropped_sz,1);
              if(im_cropped) {delete []im_cropped; im_cropped = 0;}
              //****************the first image file************************//



              //****************the second image file************************//
              im_cropped2 = callback.getSubVolumeTeraFly(image2_folder_path.toStdString(),xb,xn+1,yb,yn+1,zb,zn+1);
              QString outimg_file2;
              outimg_file2=fileSaveFolder1+"/"+"Y."+QString(numofrandom)+"."+QString(outimg_file1);
              simple_saveimage_wrapper(callback, outimg_file2.toStdString().c_str(),(unsigned char *)im_cropped2,im_cropped_sz,1);
              if(im_cropped2) {delete []im_cropped2; im_cropped2= 0;}
              //****************the second image file************************//

            }

            QString apo_name = curPathSWC + ".random.apo";
            writeAPO_file(apo_name,file_inmarkers);
            QString linker_name = curPathSWC + ".random.ano";
            QFile qf_anofile(linker_name);
            if(!qf_anofile.open(QIODevice::WriteOnly))
            {
                v3d_msg("Cannot open file for writing!");
                return false;
            }
            QTextStream out(&qf_anofile);
            out << "SWCFILE=" << QFileInfo(curPathSWC).fileName()<<endl;
            out << "APOFILE=" << QFileInfo(apo_name).fileName()<<endl;
            //cout<< "Save the linker file to:"<< linker_name << endl;

         }
       }
    else if (func_name == tr("help"))
      {
        v3d_msg("To be implemented.");
      }
      else return false;
      return true;
}
QStringList importFileList_addnumbersort(const QString & curFilePath)
{
    QStringList myList;
    myList.clear();
    QStringList imgSuffix;
    imgSuffix<<"*.swc"<<"*.eswc"<<"*.SWC"<<"*.ESWC";
    QDir dir(curFilePath);
    if(!dir.exists())
    {
        cout <<"Cannot find the directory";
        return myList;
    }
    foreach(QString file, dir.entryList(imgSuffix, QDir::Files, QDir::Name)) // matching the filename to file
    {
        myList += QFileInfo(dir, file).absoluteFilePath();
    }
    foreach(QString qs, myList) qDebug() << qs;
    return myList;
}

