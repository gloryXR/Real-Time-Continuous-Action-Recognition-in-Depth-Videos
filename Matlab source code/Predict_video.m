%% predict the probility of each video sequences
function  Predict_video()

Data_set = 1;
act_num = 21;
load('list_test_data.mat');
data_list = list_test_data;
load('aver.mat');
load('part.mat');
t = 7;
RR = 31;

for r = 1:RR
    mkdir('probability',num2str(r));
    load(['forest/forest',num2str(r),'.mat'],'Forest');
    %mkdir('probability',num2str(r));
    for i = 1:size(data_list,1)

        % get_test_data;
        test_data = Get_data(data_list(i,:),t,Data_set,aver);
        
        temp = test_data(:,1);
        for p = 1:5
            if part(r,p) ~=0
               temp = [temp,test_data(:,(part(r,p)-1)*180+2:(part(r,p)*180)+1)];
            else
               break;
            end
        end
        test_data = temp;

        % predict
        predict_result = Get_video_pro(test_data,act_num,Forest);
        save(['probability/',num2str(r),'/aa_',num2str(data_list(i,1)),num2str(data_list(i,2)),num2str(data_list(i,3))],'predict_result');
    end
   

end;


%% predict the probility of each frame in video
%  data: the feature of video
%  probility_distribution: the probility of video
%  probility_distribution(i,j): the ith frame  the probility of jth action
function probility_distribution = Get_video_pro(data,act_num,Forest_all)

% the number of frame in video 
l = size(data,1);

probility_distribution = zeros(l,act_num);

for i = 1:l
    p = Get_frame_pro(data(i,:),act_num,Forest_all);
    probility_distribution(i,:) = p;
end;


%% predict the probility of one frame
%  data: the feature of frame;
%  act_num: the number of aciton;
function proo = Get_frame_pro(data,act_num,Forest_all)


% the number of Forest;
l = size(Forest_all,2);
% pro(i,j) the probility of frame is jth action from ith tree 
proo = zeros(l,act_num);

for i = 1:l
    node = 1;
    tree = Forest_all{i};
    while(~tree{node+1}.isleaf)
        a = tree{node+1}.feature;
        if(data(a(1)+1) >= a(2))
            node = 2*node + 1;
        else
            node = 2*node ;
        end;
    end;
    proo(i,1:size(tree{node+1}.prob,2)) = tree{node+1}.prob;
end;

if l > 1
proo = sum(proo)/l;
end;