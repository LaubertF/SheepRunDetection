function [combinedData] = parse(data,showcharts)
%% find SYNC symbol for raw data
indx = strfind(data,'7E00');  %predpokladame, ze prijaty packet ma max dlzku payloadu 255B
if (isempty(indx) ~= 1)
   begin = indx(1);
   data(1:begin-1)=[];  %vymazem nekompletne bajty zo zaciatku RX spravy
   indx = strfind(data,'7E00');  %aktualizujem data, kedze sa mohol zaciatok vymazat a vsetko sa posunulo...
end
%% raw data to packet + spojim rozbite packety, ktore vznikli nedokonalim detekovanim zaciatkov packetu
n=1;
i=1;
lenK=0;
spajanie = 0;
packet{1}=[];
dindx=diff(indx);
while (i < length(indx) & i < length(dindx))
   while(i < length(dindx))
      lenK = lenK + dindx(i);   %kumulativny pocet znakov spravy(sprav)
      %ak nespajam rozdelene packety, tak neaktualizujem lenOcakavana
      if(spajanie == 0)
         lenPacket = hex2dec(data(indx(i)+2:indx(i)+5));
         lenOcakavana = (lenPacket*2) + 8;   %ocakavany pocet hex znakov vypocitany podla udaju o dlzke packetu z packetu
      end
      %ak je vsetko ako ma byt => ocakavana velkost packetu je rovnaka ako detekovana
      if(lenOcakavana - lenK == 0)
         if (length(packet) < n)
            packet{n}=[];
         end
         packet{n} = [packet{n}, data(indx(i):indx(i+1)-1)];
         i = i + 1;
         n = n + 1;
         spajanie = 0;
         lenK = 0;
         break;
      %ocakavana velkost packetu je ina ako detekovana, treba spajat...
      else
         if (length(packet) < n)
            packet{n}=[];
         end
         packet{n} = [packet{n}, data(indx(i):indx(i+1)-1)];
         i = i + 1;
         spajanie = 1;
      end
   end
 end

%% packet to payload
for i = 1:length(packet)
      payload{i}=packet{i}(31:end-2);
end
%% payload to timestamp, accelero
packetcount = length(packet);
unixtime{1} = 0;
% najdenie prveho packetu, ktory sa zacina vacsim packetom (obsahuje timestemp)
for i = 1:packetcount
   %ak sa jedna o vacsi packet s timestamp info (vacsi ako 40 = 20B*2)
   if (length(payload{i}) > 40)
      break;
   end
end
startindx=i;
stopindx=0;
k=1;
% extrakcia dat z paylodu do timestamp a accelero
for i = startindx:packetcount
   %ak sa jedna o vacsi packet s timestamp info (vacsi ako 40 = 20B*2)
   if (length(payload{i}) > 40)
      unixtime{k} = hex2dec(payload{i}(1:8));   %extrahujem timestamp (ten je iba vo vacsom payloade)
      millis{k} = hex2dec(payload{i}(9:12))-1;  %extrahujem milisec z vacsieho payloadu + uprava cislovania od 0
      ax{k} = typecast(uint16(base2dec(payload{i}(13:16), 16)), 'int16');  %hex2dec(payload{i}(13:16));
      ay{k} = typecast(uint16(base2dec(payload{i}(17:20), 16)), 'int16');  %hex2dec(payload{i}(17:20));
      az{k} = typecast(uint16(base2dec(payload{i}(21:24), 16)), 'int16');  %hex2dec(payload{i}(21:24));
      
      gx{k} = typecast(uint16(base2dec(payload{i}(25:28), 16)), 'int16');
      gy{k} = typecast(uint16(base2dec(payload{i}(29:32), 16)), 'int16');
      gz{k} = typecast(uint16(base2dec(payload{i}(33:36), 16)), 'int16');      
      
      mgx{k} = typecast(uint16(base2dec(payload{i}(37:40), 16)), 'int16');
      mgy{k} = typecast(uint16(base2dec(payload{i}(41:44), 16)), 'int16');
      mgz{k} = typecast(uint16(base2dec(payload{i}(45:48), 16)), 'int16');      
      stopindx = k;
      k = k + 1;
   else
      unixtime{k} = unixtime{k-1};  %timestamp zistim z predch. packetu
      millis{k} = hex2dec(payload{i}(1:4))-1;   %extrahujem iba milisec z mensieho payloadu + uprava cislovania od 0
      ax{k} = typecast(uint16(base2dec(payload{i}(5:8), 16)), 'int16'); %hex2dec(payload{i}(5:8));
      ay{k} = typecast(uint16(base2dec(payload{i}(9:12), 16)), 'int16'); %hex2dec(payload{i}(9:12));
      az{k} = typecast(uint16(base2dec(payload{i}(13:16), 16)), 'int16'); %hex2dec(payload{i}(13:16));
      
      gx{k} = typecast(uint16(base2dec(payload{i}(17:20), 16)), 'int16');
      gy{k} = typecast(uint16(base2dec(payload{i}(21:24), 16)), 'int16');
      gz{k} = typecast(uint16(base2dec(payload{i}(25:28), 16)), 'int16');      
      
      mgx{k} = typecast(uint16(base2dec(payload{i}(29:32), 16)), 'int16');
      mgy{k} = typecast(uint16(base2dec(payload{i}(33:36), 16)), 'int16');
      mgz{k} = typecast(uint16(base2dec(payload{i}(37:40), 16)), 'int16');       
      k = k + 1;
   end
end
packetcount = stopindx;   % dalsie vzorky od posledneho vacsieho packetu neberem do uvahy
for i = 1:packetcount
   m(i)=millis{i};
   u(i)=unixtime{i};
   accx(i) = ax{i};
   accy(i) = ay{i};
   accz(i) = az{i};

   gyrx(i) = gx{i};
   gyry(i) = gy{i};
   gyrz(i) = gz{i};

   magx(i) = mgx{i};
   magy(i) = mgy{i};
   magz(i) = mgz{i};

end
peak = diff(m);
removeindx = find(peak == -75);  %najdem vsetky vzorky, ktore maju vacsiu vzork. freq ako vascina vzoriek
m(removeindx)=[]; %odstranim tieto vzorky
u(removeindx)=[];
accx(removeindx)=[];
accy(removeindx)=[];
accz(removeindx)=[];

gyrx(removeindx)=[];
gyry(removeindx)=[];
gyrz(removeindx)=[];

magx(removeindx)=[];
magy(removeindx)=[];
magz(removeindx)=[];
%% vypocitanie spravneho datetime na zaklade unixtime a millis
SAMPLE_PERIOD = 1/75;  %herze
ut = u + (m*SAMPLE_PERIOD);
dt=datetime( ut, 'ConvertFrom', 'posixtime','Format','dd-MM-yyy HH:mm:ss.SSS' );

%% zobrazovanie

acc_level = 8;
gyro_level = 500;
sensitivity_acc = 2^15;
sensitivity_gyro = 2^15;
ares=acc_level/sensitivity_acc;
gres=gyro_level/sensitivity_gyro;
mres=4912/32760;

accx = double(accx)*ares;
accy = double(accy)*ares;
accz = double(accz)*ares;

gyrx = double(gyrx)*gres;
gyry = double(gyry)*gres;
gyrz = double(gyrz)*gres;

magx = double(magx)*mres;
magy = double(magy)*mres;
magz = double(magz)*mres;
if showcharts
    figure(1)
    ax1=subplot(3,1,1);
    plot(dt,accx,'Color',[0, 0.7, 0.9]);     %akcelerometer x
    hold on
    plot(dt,accy,'Color',[0.7, 0.0, 0.9]);     %akcelerometer y
    hold on
    plot(dt,accz,'Color',[0.9, 0.7, 0.0]);     %akcelerometer z
    legend({ 'x', 'y', 'z'})
    
    ax2=subplot(3,1,2);
    plot(dt,gyrx,'Color',[0, 0.7, 0.9]);     %gyro z
    hold on
    plot(dt,gyry,'Color',[0.7, 0.0, 0.9]);     %gyro y
    hold on
    plot(dt,gyrz,'Color',[0.9, 0.7, 0.0]);     %gyro x
    legend({ 'x', 'y', 'z'})
    
    ax3=subplot(3,1,3);
    plot(dt,magx,'Color',[0, 0.7, 0.9]);     %mag z
    hold on
    plot(dt,magy,'Color',[0.7, 0.0, 0.9]);     %mag y
    hold on
    plot(dt,magz,'Color',[0.9, 0.7, 0.0]);     %mag x
    legend({ 'x', 'y', 'z'})
    
    linkaxes([ax1,ax2,ax3],'x');   %pri zoomovani su grafy previazane v x-osy
    
    figure(2)
    ax4=subplot(3,1,1);
    plot(dt,accx,'Color',[0, 0.7, 0.9]);     %akcelerometer x
    legend({'x'})
    
    ax5=subplot(3,1,2);
    plot(dt,accy,'Color',[0.7, 0.0, 0.9]);     %akcelerometer y
    legend({'y'})
    
    ax6=subplot(3,1,3);
    plot(dt,accz,'Color',[0.9, 0.7, 0.0]);     %akcelerometer z
    legend({'z'})
    
    linkaxes([ax4,ax5,ax6],'x');   %pri zoomovani su grafy previazane v x-osy
end
% Combine data into a matrix
dataMatrix = [accx', accy', accz', gyrx', gyry', gyrz', magx', magy', magz'];

% Convert datetime array to cell array of character vectors
dtCell = cellstr(datestr(dt, 'yyyy-mm-dd HH:MM:SS.FFF'));

% Define the header row
headerRow = {'datetime','accx', 'accy', 'accz', 'gyrx', 'gyry', 'gyrz', 'magx', 'magy', 'magz'};

% Combine header row and data
combinedData = [headerRow; [dtCell, num2cell(dataMatrix)]];
end

