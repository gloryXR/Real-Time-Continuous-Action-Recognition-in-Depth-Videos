function MSR_SVM()
%% initialize variables
Data_set = 1;
% the number of aciton
action_num = 21;
% the ratio of experiment data
s  = 5;
% the length of trajectory(2*t + 1)
t = 7;


% Get the list of primitive data
if Data_set == 1
    
    J =[ 7 7 7 4 3 3 3  1 8  10 2 9  11 5  14 16 6  15 17; 5 6 4 3 1 2 20 8 10 12 9 11 13 14 16 18 15 17 19];
    
    filename = 'skeleton_data/MSR_ske/1_JiangExperimentFileList.txt';
    D = Read_txt(filename);
    [temp_list_train_data,temp_list_test_data] = Data_list(D,s);
    list_train_data = temp_list_train_data;
    list_test_data = temp_list_test_data;
end

save('list_test_data.mat','list_test_data');
save('list_train_data.mat','list_train_data');

aver = average_limbs(Data_set,J);
save('aver','aver');

%Get the train data
train_data = Get_data(list_train_data,t,Data_set,aver);
save('train_data','train_data');

%get the train label and inst
train_label=train_data(:,1);
train_inst=train_data(:,2:901);

%libsvm train
%model=svmtrain(train_label,train_inst,'-b 1');

rows = size(list_test_data,1);

%create two array for comparing the predict label and real label
result = zeros(rows,1);
real_label = list_test_data(:,1);

for i=1:rows
%     %Get the test data
%     test_data=Get_data(list_test_data(i,:),t,Data_set,aver);
% 
%     %get the test label and inst
%     [tdata_line,tdata_column]=size(test_data);
%     
%     test_inst=test_data(:,2:901);
% 
%     %libsvm predict
%     [predicted_label,accuracy,pro_result]=svmpredict(test_data(:,1),test_inst,model,'-b 1');

    load(['probability/svm/pro_result',num2str(list_test_data(i,1)),'_',num2str(list_test_data(i,2)),'_',num2str(list_test_data(i,3))],'pro_result');
    %get the result possibility
    if size(pro_result,1) > 1
        result_probability = sum(pro_result);
    else
        result_probability = pro_result;
    end
    
    %get the targetlabel
    temp=result_probability(1);
    result(i)=1;
    for j=2:20
        if(temp<result_probability(j))
            temp=result_probability(j);
            result(i)=j;
        end
    end
end

A = result - real_label;

accuracy = sum(A==0)/numel(A)*100;
display(accuracy);
save('result_svm','result');




%function MSR_SVM end

%% read object_list_data
%  filename: the file path;
function list = Read_txt(filename)

fd = fopen(filename);

l = fgetl(fd);
i = 1;
list = zeros(557,3);
while ischar(l)
    list(i,:) = [str2num(l(2:3)),str2num(l(6:7)),str2num(l(10:11))];
    l = fgetl(fd);
    i = i+1;
end;

function [list_train_data,list_test_data] = Data_list(data,s)

list_train_data = [];
list_test_data = [];

for i = 1:size(data,1)
    if data(i,2) <= 2*s
    if mod(data(i,2),2) == 1
        list_train_data = [list_train_data;data(i,:)];
    else
        list_test_data = [list_test_data;data(i,:)];
    end
    end
end

