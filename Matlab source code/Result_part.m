function Result_part()

RR = 31;
act_num = 20;
load('list_test_data');
load('rw');

l = size(list_test_data,1);
data_list = list_test_data;
label = zeros(l,RR);
pro_result = zeros(l,act_num);
for r = 1:RR
    for i = 1:l
        load(['probability\\',num2str(r),'\\aa_',num2str(data_list(i,1)),num2str(data_list(i,2)),num2str(data_list(i,3))]);
        if size(predict_result,1) == 0
            continue;
        elseif size(predict_result,1) == 1
            temp = predict_result;
        else
            temp = sum(predict_result);
        end
        temp = temp/max(temp);
        pro_result(i,:) = pro_result(i,:) + temp(1:act_num).*rw(:,r)'; 
    end
end

label1 = zeros(l,1);
for i = 1:l
    a_label = find(pro_result(i,:) == max(pro_result(i,:)));
    label1(i) = a_label(1);
end


%aa = [list_test_data(:,1),label];
bb = [list_test_data(:,1),label1];

save('bb','bb');

A = bb(:,1)-bb(:,2);
result = sum(A==0)/numel(A);
display(result);