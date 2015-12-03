%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Matlab Script to generate the constraint matrix
% in standard form for the clique transversal problem
% Author : Tim Becker
% A is a matrix
% b,c are vectors to solve the following problem:
%   Min { c'x; Ax = b, x >= 0 }
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [A, b, c, cliques] = ConstraintMatrixForTransversal(n,p)


[Adj, ~] = BRandom(n,p,0);
AA = Adj - triu(Adj);
bg = biograph(AA,[],'ShowArrows','off','ShowWeights','off');
h = view(bg); 

MC = maximalCliques(Adj,'v2');
[~,bb] = size(MC);

CliqueConstraint = zeros(bb, n);

for i = 1:bb
    CliqueConstraint(i,:) = MC(:,i);
end
spy(CliqueConstraint)


A = [-CliqueConstraint 1*eye(bb)];

b = -ones(bb,1);

c = [ones(n,1); zeros(bb,1)];

cliques = bb;

end

