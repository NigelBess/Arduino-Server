function clearPort(port)
    a = instrfind;
    for i = 1:numel(a)
        if contains(string(a(i).name),string(port))
            fclose(a(i));
            delete(a(i));
        end
    end
end