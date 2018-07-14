clc;
clear all;
clf;
content = fileread('testkey4.tsp');
formatSpec = '%d %d %d';
data = textscan( content, formatSpec);
numData = length(data{1});
x = data{2};
y = data{3};

X_range = [min(x), max(x)];
Y_range = [min(y), max(y)];

K = 3;
ip = rand(K,2)
for i = 1:K
    ip(i,1) = ip(i,1)  * (X_range(2) - X_range(1)) + X_range(1);
    ip(i,2) = ip(i,2)  * (Y_range(2) - Y_range(1)) + Y_range(1);
end

for k = 1:K   
    % scatter(ip(k,1),ip(k,2),'x'); % print the initial value
    hold on;
   for j = 1:numData
       dist_1 = abs(x(j) - ip(1,1)) + abs(y(j) - ip(1,2));
       dist_2 = abs(x(j) - ip(2,1)) + abs(y(j) - ip(2,2));
       dist_3 = abs(x(j) - ip(3,1)) + abs(y(j) - ip(3,2));
       dis = [dist_1, dist_2, dist_3];
       [mv, mi] = max(dis);
       if mi == 1
           scatter(x(j),y(j),'bo');
       elseif mi == 2
           scatter(x(j),y(j),'bo');
       else
           scatter(x(j),y(j),'bo');
       end
   end
end

