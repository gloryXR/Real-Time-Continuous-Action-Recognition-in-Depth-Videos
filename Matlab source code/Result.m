function Result()

RR = 31;
act_num = 20;
load('list_test_data');

l = size(list_test_data,1);
data_list = list_test_data;
label = zeros(l,RR);
pro_result = zeros(l,act_num);
for r = 1:RR
    for i = 1:l
        load(['probability\\',num2str(r),'\\aa_',num2str(data_list(i,1)),num2str(data_list(i,2)),num2str(data_list(i,3))]);
        p = predict_result';
        p(:,p(21,:)==max(p)) = [];
        predict_result = p';
        if size(predict_result,1) == 0
            continue;
        elseif size(predict_result,1) == 1
            temp = predict_result*predict_result(:,21);
        else
            temp = sum(diag(exp(-5*predict_result(:,21)))*predict_result(:,1:act_num));
        end
        a_label = find(temp(1:act_num) == max(temp(1:act_num)));
        label(i,r) = a_label(1);
    end
end


aa = [list_test_data(:,1),label];

save('aa','aa');
