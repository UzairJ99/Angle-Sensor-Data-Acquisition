%Uzair Jawaid
%400114899

if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
end
clc;

s = serial('COM2'); %choose COM port
set(s, 'Baudrate', 38400); %sets the Baudrate to the same as the Esduino
s.Terminator = 'CR'; %define terminator sent by Esduino
fopen(s); %open the port
x = 1:0.1:90;
t=90; %period
while 1==1
    for n=1:t
        Y(n)=str2double(fgetl(s)); %recieves data from serial port
        plot(x(1:n),Y(1:n));
        xlabel('Time (s)'); 
        ylabel('Angle(deg)');
        title("Angle Readings vs Real Time");
        %drawnow; %to display the output continuously
        pause(0.01);
    end
end