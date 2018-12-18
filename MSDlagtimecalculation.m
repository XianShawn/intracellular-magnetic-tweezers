N = 369;

for j = 1:N
    for i = 1:N
        if i+j < N+1
            a(i,j) = (data(i+j,1)-data(i,1))^2;
        end
       %data_pro(j,1) = mean(a(j,1:end)); 
    end
end

for i = 1:N,
     data_pro(i,1) = mean(a(i,1:N-i));   
end


%figure(1), plot(data_pro(1:N));
%figure(2), plot(loglog(data_pro(1:100)));