%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Matlab Script to generate the constraint matrix
% in standard form for the bilevel clique interdiction problem
% Author : Tim Becker
% A is a matrix
% b,c are vectors to solve the following problem:
%   Min { c'x; Ax = b, x >= 0 }
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [A, b, c, cliques] = ConstraintMatrix(n, Q, R, cc, d,p)


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
%Find a few cliques here and add their constraints
%Then output constraint matrix...

f = zeros(1,n); %x
ff = zeros(1,n); %z


for i = 1:n
    f(i) = floor(cc(i));
    ff(i) = floor(d(i));
end
    
f = [f zeros(1,n + bb) 1 0 zeros(1,n + bb)];
ff = [zeros(1,n + bb) ff 0 1 zeros(1,n + bb)];

A = [f;ff;eye(n) zeros(n,bb) eye(n) zeros(n,2) eye(n) zeros(n,bb)];

A = [A; -CliqueConstraint eye(bb) zeros(bb,n) zeros(bb,n+2) eye(bb)];

b = [R; Q; ones(n,1); zeros(bb,1)];

c = [zeros(n,1); ones(n + bb,1); zeros(n+bb+2,1)];

cliques = bb;

end

