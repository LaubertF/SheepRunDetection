function [data] = gatherData(device)
    data = zeros(1,524288);%145/tickrate hodin
    n = 1;
    ButtonHandle = uicontrol('Style', 'PushButton', ...
                             'String', 'Stop loop', ...
                             'Callback', 'delete(gcbf)');
    
    while(true)
        if ~ishandle(ButtonHandle)
            disp('Stop recording');
            break;
        end  
        drawnow()
       if (device.NumBytesAvailable > 0)
          %disp("!!!!!!");
          data(n) = read(device, 1, 'uint8');
          n = n+1;
       end
    end
    data = dec2hexArray(data,2);
end

