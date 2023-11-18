function [getter, S] = timestamps
    timeStamp = [];
    function recordtime(~, ~)
      timeStamp(end+1) = now;
    end
    function ts = getterfcn(varargin)
      if nargin > 0
        timeStamp = [];
      end
      ts = timeStamp;
    end
    S.hFig = figure('menu','none',...
                'pos', [200 200 320 50]);
    set(S.hFig,'KeyPressFcn',@recordtime);
    S.th = uicontrol('Style', 'text', 'Position', [10 10 300 30], ...
                  'String', 'Push button to record Time Stamp');
    S.bh = uicontrol('Style', 'push', 'Position', [10 50 30 30], ...
                  'String', 'Timestamp', 'Callback', @recordtime);
    getter = @getterfcn;
  end