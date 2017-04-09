%% get the train or test data  
function [train_data, action_data_index] = Get_data(list, t, Data_set, aver)

if Data_set == 1
   J=[ 7 7 7 4 3 3 3  1 8  10 2 9  11 5  14 16 6  15 17;
       5 6 4 3 1 2 20 8 10 12 9 11 13 14 16 18 15 17 19];
   % lo = [6,7,8,9,10,13,14,15,16,19,20,21,26,27,28,29,30,13,34,35,36,39,40,41,46,47,48,49,50,53,54,55,56,59,60,61];
    lo = [10,14,30,34,50,54,  9,13,29,33,49,53,  16,20,36,40,56,60,  15,19,35,39,55,59,   8,21,28,41,48,61];
    a  = zeros(20,1);
    action_data_index = zeros(20,100,2);
end


% get the training data
train_data = [];

for i = 1:size(list,1)
    
    A = Read_data(list(i,:), Data_set);
    if numel(A) == 0
        continue;
    end
    A = normi(A,aver,J);
  %  A = Length_normi(A);
    
    l = size(A,1);
    len = 1 + (6*8 + 6*8 + 6*t*2)*5;
    tempdata = ones(l-2*t,len);
    
    for j = t+1:l-t
        count = 1;
        for p = 1:5
            for k = 1:6
                for p1 = 1:5
                    if p == p1
                       continue;
                    end                 
                    tempdata(j-t,count+1) = A(j,lo((p-1)*6+k))-A(j,lo((p1-1)*6 + floor((k-1)/2)*2 + 1));
                    tempdata(j-t,count+2) = A(j,lo((p-1)*6+k))-A(j,lo((p1-1)*6 + floor((k-1)/2)*2 + 2));
                    count = count + 2;
                end
            end
            
            % trajectory
            for k = 1:6
                tra = A(j-t:j+t,lo((p-1)*6+k));
                d = fft(tra);
                tempdata(j-t,count+1:count+4) = real(d(2:5));
                tempdata(j-t,count+5:count+8) = imag(d(2:5));
                count = count + 8;
            end
            
            % dynamic feature 
            for tt = 1:t
                for k = 1:6
                    tempdata(j-t,count+1) = A(j,lo((p-1)*6+k))-A(j-tt,lo((p-1)*6+k));
                    tempdata(j-t,count+2) = A(j,lo((p-1)*6+k))-A(j+tt,lo((p-1)*6+k));
                    count = count + 2; 
                end
            end
        end
        tempdata(j-t,1) = A(j,1); 
    end
    train_data = [train_data;tempdata];
      
end
end


function B = normi(A,av,J)

    l = size(A,1);

    B  = A;

    for f = 1:l
        for i = 1: 19
            st = [A(f,J(1,i)+1),A(f,J(1,i)+21),A(f,J(1,i)+41)];
            en = [A(f,J(2,i)+1),A(f,J(2,i)+21),A(f,J(2,i)+41)];
            r = st-en;
            c = -1*av(i)*r/sqrt(r*r');
            B(f,J(2,i)+1)=B(f,J(1,i)+1)+ c(1);
            B(f,J(2,i)+21)=B(f,J(1,i)+21)+ c(2);
            B(f,J(2,i)+41)=B(f,J(1,i)+41)+ c(3);
        end;
    end;

end

function w = computer_weight(B)

C = B;
D = zeros(3,60);
for j = 1:60
    if j <= 20
       temp = C(:,8);
    elseif j <= 40
       temp = C(:,28);
    elseif j <= 60
       temp = C(:,48);
    end;
    D(:,j) = C(:,j+1) - temp;
end


t = D(1,:) - D(2,:);
w = sum(t*t');
t = D(1,:) - D(3,:);
w = w + sum(t*t');
w = w/2;

end