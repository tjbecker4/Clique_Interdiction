% BRandom.m
%
% Zachary Rubenstein
%
% Modified by Cynthia Wood
%
% Generates the adjacency matrix for a Bernoulli random graph.
%
% usage A = BRandom(n, p, directed)
% whereas:
% n is the number of vertices in the graph
% p is the probability that any given set of two vertices is
% connected.
% directed is a boolean variable: true if the graph to be generated is
% directed, false if the graph is to be undirected.
%
% example: A = BRandom(100, .5, 1)
function [A  m] = BRandom(n, p, directed)

    A = zeros(n,n);
    
    if directed
        
        for node1 = 1:n
        
            for node2 = setdiff(1:n, node1)
            
                if rand < p

                    A(node1, node2) = 1;

                end
                
            end
            
        end
        
    else
        
        for node1 = 1:n-1
            
            for node2 = node1+1:n

                if rand < p

                    A(node1, node2) = 1;

                    A(node2, node1) = 1;

                end
                
            end
            
        end
        
    end
    
    m = nnz(A)/2;
    

end