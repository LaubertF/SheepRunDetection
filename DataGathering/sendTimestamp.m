function sendTimestamp(device)
part = '7E 00 22 10 00 00 13 A2 00 41 A4 A1 6B FF FE 00 C0 74 69 6D 65 73 74 61 6D 70 3D';
    p = uint8(sscanf(part,'%x'))';
    
    while true
        unixtm = posixtime(datetime(datestr(now)));
        pause(0.05);
        unixtm2 = posixtime(datetime(datestr(now)));
        
        if unixtm ~= unixtm2
            p = [p, uint8(num2str(unixtm2))];
            break;
        end
    end
    
    p = [p, computeCRC(p(4:end))];
    %% odoslanie presneho casu na COM port
    write(device,p,'uint8');
end

