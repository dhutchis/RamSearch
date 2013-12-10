% Random Search Ramsey Number

%Initialize Ramsey Number
r = 6;
i = 1;
node_array = cell(20,4);
edge_array = cell(20,6);
good = zeros(170,1);
answer = zeros(50,1);

% E&S upper bound
up_bound = nchoosek(6,3) 

while i < 500 && r < up_bound
    %generate nodes of sub graph
    nodes = randperm(r,4);
    node_array{i,1} = nodes(1,1);
    node_array{i,2} = nodes(1,2);
    node_array{i,3} = nodes(1,3);
    node_array{i,4} = nodes(1,4);
    %generate edges of sub graph 
    %0 no edge, 1 edge
    %12 13 14 23 24 34
    edges = randi([0,1],1,6);
    edge_array{i,1} = edges(1,1);
    edge_array{i,2} = edges(1,2);
    edge_array{i,3} = edges(1,3);
    edge_array{i,4} = edges(1,4);
    edge_array{i,5} = edges(1,5);
    edge_array{i,6} = edges(1,6); 
    
    % check that it is a unique graph
    x = i;
    while x < 300 && x > 0
        x = x -1;
        if edge_array{i} ~= edge_array{i-x} && node_array{i} ~= node_array{i-x}
            good(i-x) = 1;
        end
    end
    if sum(good) <= 0
        ;
    else
        %iterate r (this is bad)
        if edges(1,1) == 1 && edges(1,2) == 1 && edges(1,3) == 1 && edges(1,4) == 1 && edges(1,5) == 1 && edges(1,6) == 1
            r = r + 1;
        else if edges(1,1) == 0 && edges(1,2) == 0 && edges(1,3) == 0 && edges(1,4) == 0 && edges(1,5) == 0 && edges(1,6) == 0
            r = r + 1;
            else r = r;
            end
        end
    end
    i=i+1;
    answer(i)=r;
end
r
%final_answer = round(mean(answer))