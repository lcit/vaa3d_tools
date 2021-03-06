function displayImage(obj)
if isfield(obj.gh.imageGUI,'Figure') && ishandle(obj.gh.imageGUI.Figure)
    figure(obj.gh.imageGUI.Figure);
    
    if isfield(obj.state.display,'colormap') & length(obj.state.display.colormap)>1
        colormap(obj.state.display.colormap);
    else
        colormap(gray);
    end
    hold off;
    z=obj.state.display.currentz;
    imagedata='nothing';
    cla;
       if (obj.state.display.displayraw==1)  
            if (obj.state.display.ch1==1) && (obj.state.display.ch2==1) && isfield(obj.data.ch(1),'imageArray') && isfield(obj.data.ch(2),'imageArray')  && length(obj.data.ch(1).imageArray)>0 && length(obj.data.ch(2).imageArray)>0
              
                % dual color plots
                red=obj.data.ch(2).imageArray(:,:,z);
                red=single(red);
                red=(red-obj.state.display.lowpixelch2)/(obj.state.display.highpixelch2-obj.state.display.lowpixelch2);
                red(red>1)=1;
                red(red<0)=0;
                green=obj.data.ch(1).imageArray(:,:,z);
                green=single(green);
                green=(green-obj.state.display.lowpixelch1)/(obj.state.display.highpixelch1-obj.state.display.lowpixelch1);
                green(green>1)=1;
                green(green<0)=0;
                blue=zeros(size(red));
                combine=cat(3,red,green,blue);
                h=image(combine);
            else
                if (obj.state.display.ch1==1) && isfield(obj.data.ch(1),'imageArray')  && length(obj.data.ch(1).imageArray)>0 
                    h=imagesc(obj.data.ch(1).imageArray(:,:,z),[obj.state.display.lowpixelch1 obj.state.display.highpixelch1]);
                end
                if (obj.state.display.ch2==1) && isfield(obj.data.ch(2),'imageArray') && length(obj.data.ch(2).imageArray)>0
                     h=imagesc(obj.data.ch(2).imageArray(:,:,z),[obj.state.display.lowpixelch2 obj.state.display.highpixelch2]);
                end
            end
            if exist('h','var')
                set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
            end
     elseif (obj.state.display.displayfiltered==1)
            if (obj.state.display.ch1==1) && (obj.state.display.ch2==1) && isfield(obj.data.ch(1),'filteredArray') && isfield(obj.data.ch(2),'filteredArray') && length(obj.data.ch(1).filteredArray)>0 && length(obj.data.ch(2).filteredArray)>0             
                % dual color plots
                red=obj.data.ch(2).filteredArray(:,:,z);
                red=single(red);
                red=(red-obj.state.display.lowpixelch2)/(obj.state.display.highpixelch2-obj.state.display.lowpixelch2);
                red(red>1)=1;
                red(red<0)=0;
                green=obj.data.ch(1).filteredArray(:,:,z);
                green=single(green);
                green=(green-obj.state.display.lowpixelch1)/(obj.state.display.highpixelch1-obj.state.display.lowpixelch1);
                green(green>1)=1;
                green(green<0)=0;
                blue=zeros(size(red));
                combine=cat(3,red,green,blue);
                h=image(combine);
            else
                if (obj.state.display.ch1==1) && isfield(obj.data.ch(1),'filteredArray') && length(obj.data.ch(1).filteredArray)>0 
                    h=imagesc(obj.data.ch(1).filteredArray(:,:,z),[obj.state.display.lowpixelch1 obj.state.display.highpixelch1]);
                end
                if (obj.state.display.ch2==1) && isfield(obj.data.ch(2),'filteredArray') && length(obj.data.ch(2).filteredArray)>0
                     h=imagesc(obj.data.ch(2).filteredArray(:,:,z),[obj.state.display.lowpixelch2 obj.state.display.highpixelch2]);
                end
            end
            if exist('h','var')
                set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
            end
    elseif (obj.state.display.displaybinary==1)
        if (obj.state.display.ch1==1) && (obj.state.display.ch2==1) && isfield(obj.data.ch(1),'binaryArray') && isfield(obj.data.ch(2),'binaryArray') && length(obj.data.ch(1).binaryArray)>0 && length(obj.data.ch(2).binaryArray)>0
            % dual color plots
            red=obj.data.ch(2).binaryArray(:,:,z);
            red=single(red);
            green=obj.data.ch(1).binaryArray(:,:,z);
            green=single(green);
            blue=zeros(size(red));
            combine=cat(3,red,green,blue);
            h=image(combine);
        else
            if (obj.state.display.ch1==1) && isfield(obj.data.ch(1),'binaryArray') && length(obj.data.ch(1).binaryArray)>0 
                h=imagesc(obj.data.ch(1).binaryArray(:,:,z),[0 1]);
            end
            if (obj.state.display.ch2==1) && isfield(obj.data.ch(2),'binaryArray') && length(obj.data.ch(2).binaryArray)>0
                h=imagesc(obj.data.ch(2).binaryArray(:,:,z),[0 1]);
            end
        end
        if exist('h','var')
            set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
        end    
    end;
    if (obj.state.display.displayMA)
        
          hold on;
        if (obj.state.display.ch1==1) && isfield(obj.data.ch(1),'maset') && length(obj.data.ch(1).maset)>0
            ch=1;
            h=plot(double(obj.data.ch(ch).ch(ch).maset(2,:)),double(obj.data.ch(ch).ch(ch).maset(1,:)),'r.');
            set(h,'LineWidth',5);
            set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
            h=plot(double(obj.data.ch(ch).ch(ch).trimset(2,:)),double(obj.data.ch(ch).ch(ch).trimset(1,:)),'b.');
            set(h,'LineWidth',0.1);
            set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
        end
        if (obj.state.display.ch2==1) && isfield(obj.data.ch(2),'maset') && length(obj.data.ch(2).maset)>0
            ch=2;
            h=plot(double(obj.data.ch(ch).ch(ch).maset(2,:)),double(obj.data.ch(ch).ch(ch).maset(1,:)),'r.');
            set(h,'LineWidth',5);
            set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
            h=plot(double(obj.data.ch(ch).ch(ch).trimset(2,:)),double(obj.data.ch(ch).ch(ch).trimset(1,:)),'b.');
            set(h,'LineWidth',0.1);
            set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
        end
        hold on;
        indices=find(obj.data.maset(3,:)==z);
        h=plot(double(obj.data.maset(1,indices)),double(obj.data.maset(2,indices)),'r.');
        set(h,'ButtonDownFcn','spineanalysisimageGUI(''imageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
        set(h,'MarkerSize',1);
        indices=find(obj.data.trimset(3,:)==z);
        h=plot(double(obj.data.trimset(1,indices)),double(obj.data.trimset(2,indices)),'b.');
        set(h,'ButtonDownFcn','spineanalysisimageGUI(''imageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
        set(h,'MarkerSize',1);
    end;
%     if (obj.state.display.displayMA)
%         hold on;
%         indices=find(obj.data.segmentation(:,:,z)>0.7);
%         xindice=ceil(indices/size(obj.data.segmentation,1));
%         yindice=indices-floor(indices/size(obj.data.segmentation,1))*size(obj.data.segmentation,1);
%         h=plot(xindice,yindice,'r.');
%         set(h,'ButtonDownFcn','spineanalysisimageGUI(''imageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
%         set(h,'MarkerSize',1);
%     end;
    if isfield(obj.data,'ROI') && length(obj.data.ROI) && (obj.data.ROI.z==obj.state.display.currentz)
        hold on;
        for i=1:size(obj.data.dendrites,2)
          line(obj.data.ROI.xv,obj.data.ROI.yv);   
        end
    end
    if (obj.state.display.displaydendrites)
        hold on;
        for i=1:size(obj.data.dendrites,2)
            indices=find(obj.data.dendrites(i).voxel(3,:)==z);
            h=plot(double(obj.data.dendrites(i).voxel(2,indices)),double(obj.data.dendrites(i).voxel(1,indices)),'.');
            set(h,'color',hsv2rgb([i/size(obj.data.dendrites,2) 1.0 1.0]));
            set(h,'ButtonDownFcn','spineanalysisimageGUI(''imageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
            set(h,'MarkerSize',1); 
        end
    end
    try 
    if (obj.state.display.displayspines)
        hold on;
        if (size(obj.data.slicespines,2)>=obj.state.display.currentz) & (size(obj.data.slicespines(obj.state.display.currentz).spines,2)>0)
            for i=1:size(obj.data.slicespines(obj.state.display.currentz).spines,2)
                if ((obj.data.spines(i).edittype~=3) & ((obj.data.spines(i).edittype~=1) | (obj.state.display.displayrejectedspine)))
                    colorvec=[1 0 0];
                    index=obj.data.slicespines(obj.state.display.currentz).spines(i).index;
                    if obj.state.display.displayentirespine==1 
                        h=plot(double(obj.data.spines(index).voxel(2,3:end)),double(obj.data.spines(index).voxel(1,3:end)),'.');
                        if obj.data.spines(index).edittype==1
                            colorvec=[0.5 0.5 0.5];
                        else
                            colorvec=obj.data.spines(i).color;
                        end
                    else
                        if sum(obj.data.spines(index).voxel(1:2,1)-obj.data.spines(index).voxel(1:2,2)==[0 0]')==2
                            h=plot(obj.data.spines(index).voxel(2,1),obj.data.spines(index).voxel(1,1),'.');
                            set(h,'MarkerSize',6);
                        else                    
                            h=plot(obj.data.spines(index).voxel(2,1:2),obj.data.spines(index).voxel(1,1:2));
                            set(h,'LineWidth',3);
                        end
                        if obj.data.spines(index).edittype==1
                            colorvec=[0 1 0];
                        elseif obj.data.spines(index).edittype==2
                            colorvec=[0 0 1];
                        else
                            colorvec=[1 0 0];
                        end                    
                    end 
                    set(h,'Color',colorvec);
                    set(h,'ButtonDownFcn','spineanalysisimageGUI(''imageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
                    warning off MATLAB:divideByZero;
                    if obj.data.spines(index).len<5    % right on the axon?
                        ind1=min(obj.data.spines(index).dendis+10,size(obj.data.dendrites(obj.data.spines(index).den_ind).voxel,2));
                        ind2=max(obj.data.spines(index).dendis-10,1);
                        dx=mean(obj.data.dendrites(obj.data.spines(index).den_ind).dx(ind2:ind1));
                        dy=mean(obj.data.dendrites(obj.data.spines(index).den_ind).dy(ind2:ind1));
                        h=text(double(obj.data.spines(index).voxel(2,1))-dy*15-5,double(obj.data. spines(index).voxel(1,1))+dx*20-8,int2str(obj.data.spines(index).label));
                    else
                        h=text(double(obj.data.spines(index).voxel(2,2))*2.5-double(obj.data.spines(index).voxel(2,1))*1.5-3,double(obj.data.spines(index).voxel(1,2))*2.5-double(obj.data.spines(index).voxel(1,1))*1.5-3,int2str(obj.data.spines(index).label));
                    end
                    set(h,'Color',colorvec);
                    set(h,'ButtonDownFcn','spineanalysisimageGUI(''imageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
                end
            end
        end
    end
    catch
       disp (lasterr)
    end
    if (obj.state.display.displaymarks)
        hold on;
        for i=1:size(obj.data.marks,2)
            if (obj.data.marks(i).voxel(3,1)==obj.state.display.currentz)
                colorvec=[0 1 0];
                h=plot(obj.data.marks(i).voxel(2,1),obj.data.marks(i).voxel(1,1),'+');
                set(h,'MarkerSize',6);
                set(h,'Color',colorvec);
                set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
                ind1=min(obj.data.marks(i).dendis+10,size(obj.data.dendrites(obj.data.marks(i).den_ind).voxel,2));
                ind2=max(obj.data.marks(i).dendis-10,1);
                dx=mean(obj.data.dendrites(obj.data.marks(i).den_ind).dx(ind2:ind1));
                dy=mean(obj.data.dendrites(obj.data.marks(i).den_ind).dy(ind2:ind1));
                h=text(double(obj.data.marks(i).voxel(2,1))-dy*15-5,double(obj.data.marks(i).voxel(1,1))+dx*20-8,int2str(obj.data.marks(i).label));     
                set(h,'Color',colorvec);
                set(h,'ButtonDownFcn','spineanalysisimageGUI(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
            end
        end
    end
% if (obj.state.display.displaymarks)
%         hold on;
%         for i=1:size(obj.marks,1)
%             if (obj.marks(i,3)==obj.state.display.currentz)
%                 h=plot(obj.marks(i,1)+1,obj.marks(i,2)+1,'r+');
%                 set(h,'ButtonDownFcn','SpineAnalysisImage(''projectionimageaxes_ButtonDownFcn'',gcbo,[],guidata(gcbo))');
%                 h=text(obj.marks(i,1)+10,obj.marks(i,2)+10,int2str(i));
%                 set(h,'color',[1 0 0]);
%             end
%         end
% end        
end
