function [crc] = computeCRC(numarray)
   %vstup = numarray:   je pole cisel z kotrych chcem vypocitat CRC
   %vystup = crc:       specialny 8-bit CRC
   tmp=uint32(numarray);
   crc = 0;
   for i = 1:length(numarray)
      crc = crc + tmp(i);  
   end
   crc = uint8(255 - bitand(crc,255));
end

