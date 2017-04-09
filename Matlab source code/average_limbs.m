
function aver = average_limbs(Data_set,J)


if Data_set == 1
    A = ske(Data_set);

    aver = zeros(19,1);
    l = size(A,1);
    for frame = 1:l/20
        d = 20*(frame-1);
        for i = 1:19
            r = A(d+J(1,i),:) - A(d+J(2,i),:);
            aver(i) = aver(i) + sqrt(r*r');
        end;
    end;
    aver = aver*20/l;

end
end

function C = ske(Data_set)

    C = [];

    for i = 1:10
        B = Read_data([12,i,1],Data_set);
        for j = 1:10
            C = [C;reshape(B(j,2:61),20,3)];
        end
    end
    
end