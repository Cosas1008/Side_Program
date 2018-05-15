function R = RotationalMatrix(rxdeg,rydeg,rzdeg)

rX = deg2rad(rxdeg);
rY = deg2rad(rydeg);
rZ = deg2rad(rzdeg);

Rx = [1 0 0 ; 0 cos(rX) -sin(rX); 0 sin(rX) cos(rX)];
Ry = [cos(rY) 0 sin(rY); 0 1 0; -sin(rY) 0 cos(rY)];
Rz = [cos(rZ) -sin(rZ) 0; sin(rZ) cos(rZ) 0; 0 0 1];
R = Rz * Ry * Rx;



end