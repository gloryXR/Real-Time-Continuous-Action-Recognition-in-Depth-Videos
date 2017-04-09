function Discriminative_bodypart1()
RR = 31;
act_num = 20;

load('aa');
rw = zeros(act_num,RR);
cc = rw;
dd = rw;

A = aa(:,1);
for r = 1:RR
    B = aa(:,r+1);
    for i = 1:act_num
        c = B(A==i);
        d = A(B==i);
        if numel(c) ~= 0
        cc(i,r) = sum(c==i)/numel(c);
        end
        if numel(d) ~= 0
        dd(i,r) = sum(d==i)/numel(d);
        end
        rw(i,r) = cc(i,r) + dd(i,r);
    end
    %rw(rw(:,r)~= 0,r) = rw(rw(:,r)~=0,r) + 0.01*sum(cc(:,r))/16;
end

for i = 1:act_num
    if sum(rw(i,:)) == 0
        continue;
    end
    ma = find(rw(i,:)== max(rw(i,:)));
    if numel(ma) < 4
       [s,id]=sort(rw(i,:));
       rw(i,id(28:31)) = 10;
    else
        rw(i,ma) = 10;
    end
    
    rw(i,rw(i,:)<10) = 0;
    if sum(rw(i,:)) ~= 0
       rw(i,:) = rw(i,:)/sum(rw(i,:));
    end    
%     ma = find(rw(i,:) == max(rw(i,:)));
%     if numel(ma) == 1
%         rw(i,ma) = -10;
%         ma = find(rw(i,:) == max(rw(i,:)) | rw(i,:) == -10);
%     end 
%     rw(i,ma) = 10;
%     rw(i,rw(i,:) < 10) = 0;
%     if sum(rw(i,:)) ~= 0
%        rw(i,:) = rw(i,:)/sum(rw(i,:));
%     end
end

save('rw','rw');
save('cc','cc');
save('dd','dd');
% rw = zeros(act_num,RR);
% rw(1,6) = 1;
% rw(2,6) = 1;
% rw(3,8) = 1;
% rw(4,6) = 1;
% rw(5,7) = 1;
% rw(6,1) = 1;
% rw(7,6) = 1;
% rw(8,1) = 1;
% rw(9,1) = 1;
% rw(10,6) = 1;
% rw(11,6) = 1;
% rw(12,7) = 1;
% rw(13,6) = 1;
% rw(14,6) = 1;
% rw(15,7) = 1;
% rw(16,1) = 1;
% rw(17,1) = 1;
% rw(18,6) = 1;
% rw(19,6) = 1;
% rw(20,6) = 1;
% for i = 1:act_num
%     A = weight(i,:);
%     ma = max(A);
%     mi = min(A);
%     rw(i,:) = (A - mi)/(ma-mi);
% end

% save('rw','rw');