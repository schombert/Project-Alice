

x = [0:0.01:pi/2];
y = angle_diff(x, 200, 20);
x = x / (pi/2);
y = y / pi * 180;
y = y - 90;
plot(x, y)

function angle = angle_diff(t, p_diff, t_diff)
    p_diff = 2*pi/p_diff;
    t_diff = pi/t_diff;
    [~,n] = size(t);
    p = zeros(1,n);
    x0 = get_coord(p, t);
    x1 = get_coord(p, t + t_diff) - x0;
    x1 = x1 ./ sqrt(sum(x1.^2, 1));
    x2 = get_coord(p + p_diff, t) - x0;
    x2 = x2 ./ sqrt(sum(x2.^2, 1));
    dot = x1 .* x2;
    angle = acos(sum(dot));
end

function coord = get_coord(p, t)
    coord = [cos(p).*sin(t);
             sin(p).*sin(t);
             cos(t)];
end
