function [out] = dec2hexArray(decnum,numDigits)
   len = length(decnum);
   if (isvector(decnum) && isnumeric(decnum))
      hextmp = dec2hex(decnum,numDigits);
      n = 1;
      for i = 1:size(hextmp,1)
         out(n:n+1) = hextmp(i,:);
         n = n + 2;
      end
   else
      disp('zly vstupny format');
   end
   
end

