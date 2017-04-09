%% we read the single primitive skeleton data for file.
%  file_address: file address number
%  Dataset:  Dataset == 1: MSR
%            Dataset == 2: XX

function D = Read_data(file_address, Data_set)

if Data_set == 1
    load_name=sprintf('skeleton_data/MSR_ske/a%02d_s%02d_e%02d_skeleton3D.txt',file_address(1),file_address(2),file_address(3));

    if exist(load_name,'file') == 0
       display('read data error');
       return ;
    end
    fp=fopen(load_name);
    if (fp>0)
       A=fscanf(fp,'%f');
       fclose(fp);
    end;

    l = size(A,1)/4;
    A = reshape(A,4,l);
    D = [];
    temp = zeros(3,20);
    
    for i  = 1:l/20
        st = 20*(i-1)+1;
        en = st +19;
        t = A(1:3,st:en);
        if t == temp
            continue;
        end
        D = [D;file_address(1) t(1,:) t(2,:) t(3,:) 1 l/20];
    end
    len = size(D,1);
    D(:,size(D,2)) = len*ones(len,1);
else
    a = 1;
end

end