% ****************************************************************************
% CUI
%
% The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
%
% The use, dissemination or disclosure of data in this file is subject to
% limitation or restriction. See accompanying README and LICENSE for details.
% ****************************************************************************
function x = s_plot(inFilename)

fid = fopen(inFilename,'r');
x = fid;
if (fid ~= -1)
   displayString = strcat(inFilename, ' is being processed');
   disp(displayString)
else
   displayString = strcat(inFilename, ' could not be found');
   disp(displayString)
   return
end

% Get the Data Table Dimensions
RowsCols = fscanf(fid,'%f',2);
rows = RowsCols(1);
cols = RowsCols(2);

% Read the Cross Ranges
xr = fscanf(fid,'%f',cols);

% Pull out all of the data
i = 1;
while (i <= rows)
    dr(i) = fscanf(fid,'%f',1);
    d = fscanf(fid,'%f',cols);
    data(i,:) = d;
    i = i + 1;
end

% Create a new figure
figure;
grid on;hold on;

%[x,y] = meshgrid(dr,xr);
%mesh(x,y,data);
%
i = 1;
while (i <= length(xr))
    j = 1;
   while (j <= length(dr))
        if (data(j,i) ~= 0.0)
            outStr = 'gO';
        else
            outStr = 'rO';
        end
        plot(dr(j),xr(i),outStr)
        j = j + 1;
    end
    i = i + 1;
end

%clear all; close all;
