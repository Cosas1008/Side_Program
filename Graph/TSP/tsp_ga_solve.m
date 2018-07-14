n = 100;
content = fileread('testkey1.tsp');
formatSpec = '%d %d %d';
data = textscan( content, formatSpec);
numData = length(data{1});
xy = [data{2}, data{3}];
userConfig = struct('xy',xy);
resultStruct = tsp_ga(userConfig);