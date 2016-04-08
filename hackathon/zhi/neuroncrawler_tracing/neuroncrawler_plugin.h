/* neuroncrawler_plugin.h
 * This is a test plugin, you can use it as a demo.
 * 2016-2-16 : by Zhi Zhou
 */
 
#ifndef __NERUONCRAWLER_APP2_PLUGIN_H__
#define __NERUONCRAWLER_APP2_PLUGIN_H__

#include <QtGui>
#include <v3d_interface.h>

class neuroncrawler : public QObject, public V3DPluginInterface2_1
{
	Q_OBJECT
	Q_INTERFACES(V3DPluginInterface2_1);

public:
	float getPluginVersion() const {return 1.1f;}

	QStringList menulist() const;
	void domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent);

	QStringList funclist() const ;
	bool dofunc(const QString &func_name, const V3DPluginArgList &input, V3DPluginArgList &output, V3DPluginCallback2 &callback, QWidget *parent);
};

class neuroncrawler_app2_raw : public QDialog
    {
        Q_OBJECT

    public:
        neuroncrawler_app2_raw(V3DPluginCallback2 &cb, QWidget *parent)
        {
            image = 0;

            v3dhandle curwin = cb.currentImageWindow();
            if (curwin)
            {
                image = cb.getImage(curwin);
                listLandmarks = cb.getLandmark(curwin);
                if(listLandmarks.count() ==0)
                {
                    v3d_msg("No markers in the current image, please select a marker.");
                    return;
                }
            }

            QGridLayout * layout = new QGridLayout();
            channel_spinbox = new QSpinBox();
            channel_spinbox->setRange(1,3);
            channel_spinbox->setValue(1);
            cnntype_spinbox = new QSpinBox();
            cnntype_spinbox->setRange(1,3);
            cnntype_spinbox->setValue(2);
            bkgthresh_spinbox = new QSpinBox();
            bkgthresh_spinbox->setRange(-1, 255);
            bkgthresh_spinbox->setValue(10);
            lenthresh_editor = new QLineEdit(QString("").setNum(5));
            srratio_editor = new QLineEdit(QString("").setNum(3.0/9.0));
            isgsdt_checker = new QCheckBox();
            isgsdt_checker->setChecked(false);
            iswb_checker = new QCheckBox();
            iswb_checker->setChecked(false);
            b256_checker = new QCheckBox();
            b256_checker->setChecked(true);
            b_radius2Dchecker = new QCheckBox();
            b_radius2Dchecker->setChecked(true);

            block_spinbox = new QSpinBox();
            block_spinbox->setRange(1,2048);
            block_spinbox->setValue(1024);

            b_adapWinchecker = new QCheckBox();
            b_adapWinchecker->setChecked(false);

            raw_filepath = new QLineEdit();
            openrawFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                raw_filepath->setText(cb.getImageName(curwin));
                raw_filepath->setDisabled(true);
                openrawFile->setDisabled(true);
            }

            marker_filepath = new QLineEdit();
            openmarkerFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                openmarkerFile->setDisabled(true);
                marker_filepath->setDisabled(true);
            }

            layout->addWidget(new QLabel("color channel"),0,0);
            layout->addWidget(channel_spinbox, 0,1,1,5);
            layout->addWidget(new QLabel("background_threshold \n(if set as -1, \nthen auto-thresholding)"),1,0);
            layout->addWidget(bkgthresh_spinbox, 1,1,1,5);

            QHBoxLayout * hbox1 = new QHBoxLayout();
            hbox1->addWidget(new QLabel("auto-downsample"));
            hbox1->addWidget(b256_checker);
            hbox1->addWidget(new QLabel("use GSDT"));
            hbox1->addWidget(isgsdt_checker);
            hbox1->addWidget(new QLabel("allow gap"));
            hbox1->addWidget(iswb_checker);
            hbox1->addWidget(new QLabel("radius from 2D?"));
            hbox1->addWidget(b_radius2Dchecker);

            layout->addLayout(hbox1,2,0,1,6);

            layout->addWidget(new QLabel("cnn_type"),3,0);
            layout->addWidget(cnntype_spinbox, 3,1,1,5);
            layout->addWidget(new QLabel("length_thresh"),4,0);
            layout->addWidget(lenthresh_editor, 4,1,1,5);
            layout->addWidget(new QLabel("SR_ratio"),5,0);
            layout->addWidget(srratio_editor, 5,1,1,5);
            layout->addWidget(new QLabel("block_size"),6,0);
            layout->addWidget(block_spinbox, 6,1,1,3);
            layout->addWidget(new QLabel("adaptive size?"),6,4);
            layout->addWidget(b_adapWinchecker);

            layout->addWidget(new QLabel(QObject::tr("va3draw/raw image or tc file:")),7,0);
            layout->addWidget(raw_filepath,7,1,1,4);
            layout->addWidget(openrawFile,7,5,1,1);

            layout->addWidget(new QLabel(QObject::tr("marker file:")),8,0);
            layout->addWidget(marker_filepath,8,1,1,4);
            layout->addWidget(openmarkerFile,8,5,1,1);

            QHBoxLayout * hbox2 = new QHBoxLayout();
            QPushButton * ok = new QPushButton(" ok ");
            ok->setDefault(true);
            QPushButton * cancel = new QPushButton("cancel");
            hbox2->addWidget(cancel);
            hbox2->addWidget(ok);

            layout->addLayout(hbox2,9,0,9,6);
            setLayout(layout);
            setWindowTitle(QString("NeuronCrawler_APP2"));


            connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
            connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

            connect(channel_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(cnntype_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(bkgthresh_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));

            connect(lenthresh_editor, SIGNAL(selectionChanged ()), this, SLOT(update()));
            connect(srratio_editor, SIGNAL(selectionChanged ()), this, SLOT(update()));

            connect(isgsdt_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(iswb_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(b256_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(b_radius2Dchecker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(b_adapWinchecker, SIGNAL(stateChanged(int)), this, SLOT(update()));


            connect(block_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(openrawFile, SIGNAL(clicked()), this, SLOT(_slots_openrawFile()));
            connect(openmarkerFile, SIGNAL(clicked()), this, SLOT(_slots_openmarkerFile()));

            update();
        }

        ~neuroncrawler_app2_raw(){}

        public slots:
        void update()
        {
            channel = channel_spinbox->value();
            cnn_type = cnntype_spinbox->value();
            bkg_thresh = bkgthresh_spinbox->value();

            length_thresh = lenthresh_editor->text().toInt();
            SR_ratio = srratio_editor->text().toInt();

            isgsdt_checker->isChecked()? is_gsdt = 1 : is_gsdt = 0;
            iswb_checker->isChecked()? is_break_accept = 1 : is_break_accept = 0;
            b256_checker->isChecked()? b_256cube = 1 : b_256cube = 0;
            b_radius2Dchecker->isChecked() ? b_RadiusFrom2D = 1 : b_RadiusFrom2D = 0;
            b_adapWinchecker->isChecked() ? adap_win = 1 : adap_win = 0;

            block_size = block_spinbox->value();
            rawfilename = raw_filepath->text();
            markerfilename = marker_filepath->text();

        }

        void _slots_openrawFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Raw File/TC File"),
                                                        "",
                                                        QObject::tr("Supported file (*.raw *.RAW *.V3DRAW *.v3draw *.tc)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                raw_filepath->setText(fileOpenName);
            }
            update();

        }

        void _slots_openmarkerFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Marker File"),
                                                        "",
                                                        QObject::tr("Supported file (*.marker *.MARKER)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                marker_filepath->setText(fileOpenName);
            }
            update();

        }
    public:

        QSpinBox * channel_spinbox;
        QSpinBox * cnntype_spinbox;
        QSpinBox * bkgthresh_spinbox;
        QLineEdit * lenthresh_editor;
        QLineEdit * srratio_editor;
        QCheckBox * isgsdt_checker;
        QCheckBox * iswb_checker;
        QCheckBox * b256_checker;
        QCheckBox * b_radius2Dchecker;
        QCheckBox * b_adapWinchecker;


        QSpinBox * block_spinbox;


        QLineEdit * raw_filepath;
        QLineEdit * marker_filepath;
        QPushButton *openrawFile;
        QPushButton *openmarkerFile;

        Image4DSimple* image;
        LandmarkList listLandmarks;
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

        QString rawfilename;
        QString markerfilename;

    };

class neuroncrawler_app1_raw : public QDialog
    {
        Q_OBJECT

    public:
        neuroncrawler_app1_raw(V3DPluginCallback2 &cb, QWidget *parent)
        {
            image = 0;
            int chn_num = 3;
            v3dhandle curwin = cb.currentImageWindow();
            if (curwin)
            {
                image = cb.getImage(curwin);
                chn_num = image->getCDim();

                listLandmarks = cb.getLandmark(curwin);
                if(listLandmarks.count() ==0)
                {
                    v3d_msg("No markers in the current image, please select a marker.");
                    return;
                }
            }

            QGridLayout * layout = new QGridLayout();
            channel_spinbox = new QSpinBox();
            channel_spinbox->setRange(1,chn_num);
            channel_spinbox->setValue(1);

            bkgthresh_spinbox = new QSpinBox();
            bkgthresh_spinbox->setRange(-1, 255);
            bkgthresh_spinbox->setValue(-1);

            visthresh_spinbox = new QSpinBox();
            visthresh_spinbox->setRange(1, 255);
            visthresh_spinbox->setValue(30);

            downsample_spinbox = new QSpinBox();
            downsample_spinbox->setRange(0, 10);
            downsample_spinbox->setValue(2);

            block_spinbox = new QSpinBox();
            block_spinbox->setRange(1,2048);
            block_spinbox->setValue(1024);

            b_adapWinchecker = new QCheckBox();
            b_adapWinchecker->setChecked(false);

            raw_filepath = new QLineEdit();
            openrawFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                raw_filepath->setText(cb.getImageName(curwin));
                raw_filepath->setDisabled(true);
                openrawFile->setDisabled(true);
            }

            marker_filepath = new QLineEdit();
            openmarkerFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                openmarkerFile->setDisabled(true);
                marker_filepath->setDisabled(true);
            }

            layout->addWidget(new QLabel("color channel"),0,0);
            layout->addWidget(channel_spinbox, 0,1,1,5);
            layout->addWidget(new QLabel("background_threshold \n  (if set as -1, \nthen auto-thresholding)"),1,0);
            layout->addWidget(bkgthresh_spinbox, 1,1,1,5);
            layout->addWidget(new QLabel("visibility threshold \n  (normally do not need to change)"),2,0);
            layout->addWidget(visthresh_spinbox, 2,1,1,5);

            QHBoxLayout * hbox1 = new QHBoxLayout();
            hbox1->addWidget(new QLabel("downsample factor \n  (set 0 for auto-downsample)"));
            hbox1->addWidget(downsample_spinbox);

            layout->addLayout(hbox1,3,0,1,6);

            layout->addWidget(new QLabel("block_size"),4,0);
            layout->addWidget(block_spinbox, 4,1,1,3);
            layout->addWidget(new QLabel("adaptive size?"),4,4);
            layout->addWidget(b_adapWinchecker);

            layout->addWidget(new QLabel(QObject::tr("va3draw/raw image or tc file:")),5,0);
            layout->addWidget(raw_filepath,5,1,1,4);
            layout->addWidget(openrawFile,5,5,1,1);

            layout->addWidget(new QLabel(QObject::tr("marker file:")),6,0);
            layout->addWidget(marker_filepath,6,1,1,4);
            layout->addWidget(openmarkerFile,6,5,1,1);

            QHBoxLayout * hbox2 = new QHBoxLayout();
            QPushButton * ok = new QPushButton(" ok ");
            ok->setDefault(true);
            QPushButton * cancel = new QPushButton("cancel");
            hbox2->addWidget(cancel);
            hbox2->addWidget(ok);

            layout->addLayout(hbox2,7,0,7,6);
            setLayout(layout);
            setWindowTitle(QString("NeuronCrawler_APP1"));


            connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
            connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

            connect(channel_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(bkgthresh_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(visthresh_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(downsample_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(b_adapWinchecker, SIGNAL(stateChanged(int)), this, SLOT(update()));

            connect(block_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(openrawFile, SIGNAL(clicked()), this, SLOT(_slots_openrawFile()));
            connect(openmarkerFile, SIGNAL(clicked()), this, SLOT(_slots_openmarkerFile()));

            update();
        }

        ~neuroncrawler_app1_raw(){}

        public slots:
        void update()
        {
            channel = channel_spinbox->value();
            bkg_thresh = bkgthresh_spinbox->value();
            block_size = block_spinbox->value();
            visible_thresh = visthresh_spinbox->value();

            rawfilename = raw_filepath->text();
            markerfilename = marker_filepath->text();
            downsample_factor = downsample_spinbox->value();

            b_adapWinchecker->isChecked() ? adap_win = 1 : adap_win = 0;

            b_256cube = (downsample_factor==0) ? 1 : 0;

        }

        void _slots_openrawFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Raw File/TC File"),
                                                        "",
                                                        QObject::tr("Supported file (*.raw *.RAW *.V3DRAW *.v3draw *.tc)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                raw_filepath->setText(fileOpenName);
            }
            update();

        }

        void _slots_openmarkerFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Marker File"),
                                                        "",
                                                        QObject::tr("Supported file (*.marker *.MARKER)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                marker_filepath->setText(fileOpenName);
            }
            update();

        }
    public:

        QSpinBox * channel_spinbox;
        QSpinBox * bkgthresh_spinbox;
        QSpinBox * visthresh_spinbox;
        QSpinBox * downsample_spinbox;
        QCheckBox * b_adapWinchecker;

        QSpinBox * block_spinbox;


        QLineEdit * raw_filepath;
        QLineEdit * marker_filepath;
        QPushButton *openrawFile;
        QPushButton *openmarkerFile;

        Image4DSimple* image;
        LandmarkList listLandmarks;
        int  bkg_thresh;
        int  channel;
        int  visible_thresh;
        int  b_256cube;
        int block_size;
        int downsample_factor;
        int adap_win;

        QString rawfilename;
        QString markerfilename;

    };

class neuroncrawler_most_raw : public QDialog
    {
        Q_OBJECT

    public:
        neuroncrawler_most_raw(V3DPluginCallback2 &cb, QWidget *parent)
        {
            image = 0;
            int chn_num = 3;
            v3dhandle curwin = cb.currentImageWindow();
            if (curwin)
            {
                image = cb.getImage(curwin);
                chn_num = image->getCDim();

                listLandmarks = cb.getLandmark(curwin);
                if(listLandmarks.count() ==0)
                {
                    v3d_msg("No markers in the current image, please select a marker.");
                    return;
                }
            }

            QGridLayout * layout = new QGridLayout();
            channel_spinbox = new QSpinBox();
            channel_spinbox->setRange(1,chn_num);
            channel_spinbox->setValue(1);

            bkgthresh_spinbox = new QSpinBox();
            bkgthresh_spinbox->setRange(0, 255);
            bkgthresh_spinbox->setValue(10);

            seedwin_spinbox = new QSpinBox();
            seedwin_spinbox->setRange(1, 100);
            seedwin_spinbox->setValue(20);

            slipwin_spinbox = new QSpinBox();
            slipwin_spinbox->setRange(1, 100);
            slipwin_spinbox->setValue(20);

            block_spinbox = new QSpinBox();
            block_spinbox->setRange(1,2048);
            block_spinbox->setValue(1024);

            b_adapWinchecker = new QCheckBox();
            b_adapWinchecker->setChecked(false);

            raw_filepath = new QLineEdit();
            openrawFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                raw_filepath->setText(cb.getImageName(curwin));
                raw_filepath->setDisabled(true);
                openrawFile->setDisabled(true);
            }

            marker_filepath = new QLineEdit();
            openmarkerFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                openmarkerFile->setDisabled(true);
                marker_filepath->setDisabled(true);
            }

            layout->addWidget(new QLabel("color channel"),0,0);
            layout->addWidget(channel_spinbox, 0,1,1,5);
            layout->addWidget(new QLabel("background_threshold)"),1,0);
            layout->addWidget(bkgthresh_spinbox, 1,1,1,5);
            layout->addWidget(new QLabel("seed window size)"),2,0);
            layout->addWidget(seedwin_spinbox, 2,1,1,5);

            QHBoxLayout * hbox1 = new QHBoxLayout();
            hbox1->addWidget(new QLabel("slip window size"));
            hbox1->addWidget(slipwin_spinbox);

            layout->addLayout(hbox1,3,0,1,6);


            layout->addWidget(new QLabel("block_size"),4,0);
            layout->addWidget(block_spinbox, 4,1,1,3);
            layout->addWidget(new QLabel("adaptive size?"),4,4);
            layout->addWidget(b_adapWinchecker);

            layout->addWidget(new QLabel(QObject::tr("va3draw/raw image:")),5,0);
            layout->addWidget(raw_filepath,5,1,1,4);
            layout->addWidget(openrawFile,5,5,1,1);

            layout->addWidget(new QLabel(QObject::tr("marker file:")),6,0);
            layout->addWidget(marker_filepath,6,1,1,4);
            layout->addWidget(openmarkerFile,6,5,1,1);

            QHBoxLayout * hbox2 = new QHBoxLayout();
            QPushButton * ok = new QPushButton(" ok ");
            ok->setDefault(true);
            QPushButton * cancel = new QPushButton("cancel");
            hbox2->addWidget(cancel);
            hbox2->addWidget(ok);

            layout->addLayout(hbox2,7,0,7,6);
            setLayout(layout);
            setWindowTitle(QString("NeuronCrawler_MOST"));


            connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
            connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

            connect(channel_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(bkgthresh_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(seedwin_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(slipwin_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));

            connect(b_adapWinchecker, SIGNAL(stateChanged(int)), this, SLOT(update()));

            connect(block_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(openrawFile, SIGNAL(clicked()), this, SLOT(_slots_openrawFile()));
            connect(openmarkerFile, SIGNAL(clicked()), this, SLOT(_slots_openmarkerFile()));

            update();
        }

        ~neuroncrawler_most_raw(){}

        public slots:
        void update()
        {
            channel = channel_spinbox->value();
            bkg_thresh = bkgthresh_spinbox->value();



            block_size = block_spinbox->value();
            seed_win = seedwin_spinbox->value();

            rawfilename = raw_filepath->text();
            markerfilename = marker_filepath->text();

            slip_win = slipwin_spinbox->value();

            b_adapWinchecker->isChecked() ? adap_win = 1 : adap_win = 0;

        }

        void _slots_openrawFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Raw File/TC File"),
                                                        "",
                                                        QObject::tr("Supported file (*.raw *.RAW *.V3DRAW *.v3draw *.tc)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                raw_filepath->setText(fileOpenName);
            }
            update();

        }

        void _slots_openmarkerFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Marker File"),
                                                        "",
                                                        QObject::tr("Supported file (*.marker *.MARKER)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                marker_filepath->setText(fileOpenName);
            }
            update();

        }
    public:

        QSpinBox * channel_spinbox;
        QSpinBox * bkgthresh_spinbox;
        QSpinBox * seedwin_spinbox;
        QSpinBox * slipwin_spinbox;
        QCheckBox * b_adapWinchecker;

        QSpinBox * block_spinbox;


        QLineEdit * raw_filepath;
        QLineEdit * marker_filepath;
        QPushButton *openrawFile;
        QPushButton *openmarkerFile;

        Image4DSimple* image;
        LandmarkList listLandmarks;
        int  bkg_thresh;
        int  channel;
        int  seed_win;
        int  slip_win;
        int block_size;
        int downsample_factor;
        int adap_win;

        QString rawfilename;
        QString markerfilename;

    };

class neuroncrawler_neutube_raw : public QDialog
    {
        Q_OBJECT

    public:
        neuroncrawler_neutube_raw(V3DPluginCallback2 &cb, QWidget *parent)
        {
            image = 0;
            v3dhandle curwin = cb.currentImageWindow();
            if (curwin)
            {
                image = cb.getImage(curwin);
                listLandmarks = cb.getLandmark(curwin);
                if(listLandmarks.count() ==0)
                {
                    v3d_msg("No markers in the current image, please select a marker.");
                    return;
                }
            }

            QGridLayout * layout = new QGridLayout();
            block_spinbox = new QSpinBox();
            block_spinbox->setRange(1,2048);
            block_spinbox->setValue(1024);

            b_adapWinchecker = new QCheckBox();
            b_adapWinchecker->setChecked(false);

            raw_filepath = new QLineEdit();
            openrawFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                raw_filepath->setText(cb.getImageName(curwin));
                raw_filepath->setDisabled(true);
                openrawFile->setDisabled(true);
            }

            marker_filepath = new QLineEdit();
            openmarkerFile = new QPushButton(QObject::tr("..."));
            if(curwin)
            {
                openmarkerFile->setDisabled(true);
                marker_filepath->setDisabled(true);
            }

            layout->addWidget(new QLabel("block_size"),4,0);
            layout->addWidget(block_spinbox, 4,1,1,3);
            layout->addWidget(new QLabel("adaptive size?"),4,4);
            layout->addWidget(b_adapWinchecker,4,5);

            layout->addWidget(new QLabel(QObject::tr("va3draw/raw image:")),5,0);
            layout->addWidget(raw_filepath,5,1,1,4);
            layout->addWidget(openrawFile,5,5,1,1);

            layout->addWidget(new QLabel(QObject::tr("marker file:")),6,0);
            layout->addWidget(marker_filepath,6,1,1,4);
            layout->addWidget(openmarkerFile,6,5,1,1);

            QHBoxLayout * hbox2 = new QHBoxLayout();
            QPushButton * ok = new QPushButton(" ok ");
            ok->setDefault(true);
            QPushButton * cancel = new QPushButton("cancel");
            hbox2->addWidget(cancel);
            hbox2->addWidget(ok);

            layout->addLayout(hbox2,7,0,7,6);
            setLayout(layout);
            setWindowTitle(QString("NeuronCrawler"));


            connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
            connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

            connect(block_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(openrawFile, SIGNAL(clicked()), this, SLOT(_slots_openrawFile()));
            connect(openmarkerFile, SIGNAL(clicked()), this, SLOT(_slots_openmarkerFile()));

            connect(b_adapWinchecker, SIGNAL(stateChanged(int)), this, SLOT(update()));


            update();
        }

        ~neuroncrawler_neutube_raw(){}

        public slots:
        void update()
        {
            block_size = block_spinbox->value();
            rawfilename = raw_filepath->text();
            markerfilename = marker_filepath->text();

            b_adapWinchecker->isChecked() ? adap_win = 1 : adap_win = 0;

        }

        void _slots_openrawFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Raw File/TC File"),
                                                        "",
                                                        QObject::tr("Supported file (*.raw *.RAW *.V3DRAW *.v3draw *.tc)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                raw_filepath->setText(fileOpenName);
            }
            update();

        }

        void _slots_openmarkerFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Marker File"),
                                                        "",
                                                        QObject::tr("Supported file (*.marker *.MARKER)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                marker_filepath->setText(fileOpenName);
            }
            update();

        }
    public:


        QSpinBox * block_spinbox;
        QCheckBox * b_adapWinchecker;


        QLineEdit * raw_filepath;
        QLineEdit * marker_filepath;
        QPushButton *openrawFile;
        QPushButton *openmarkerFile;

        Image4DSimple* image;
        LandmarkList listLandmarks;
        int block_size;
        int adap_win;

        QString rawfilename;
        QString markerfilename;

    };

#endif

