%% 
clc;

% input degree
ro = linspace(0,30);
position = [-240; -138; 0];
rect = [0,0,0;-240,0,0;-240,-138,0;0,-138,0;0,0,0];
answer = zeros(3,30*30*30);

h = animatedline; % animation

% rotation
for j = 1:100
    for k = 1:200
        if( k <= 100)
            R = RotationalMatrix(0,ro(j),ro(k));
            PoR = R * position;
            answer(:,k+j-1) = PoR;
            addpoints(h,answer(1,k+j-1),answer(2,k+j-1),answer(3,k+j-1));
            drawnow
        else
            addpoints(h,answer(1,200 - k+j),answer(2,200 - k+j),answer(3,200 - k+j));
            drawnow
        end
    end
end

figure;
xlabel('x');
ylabel('y');
zlabel('z');   
fill3(rect(:,1),rect(:,2),rect(:,3),0.1);
hold on;
Rs = RotationalMatrix(ro(100),ro(100),ro(100));
for i = 1:5
    P = R * rect(i,:)';
    rect(i,:) = P';
end
% graph

xlabel('x');
ylabel('y');
zlabel('z');   
scatter3(answer(1,:),answer(2,:),answer(3,:));
hold on;
fill3(rect(:,1),rect(:,2),rect(:,3),0.5);
