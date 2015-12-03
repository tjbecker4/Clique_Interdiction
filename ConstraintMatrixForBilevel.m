%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Matlab Script to generate the constraint matrix
% in standard form for the bilevel clique interdiction problem
% Author : Tim Becker
% A is a matrix
% b,c are vectors to solve the following problem:
%   Min { c'x; Ax = b, x >= 0 }
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [A, b, c, cliques] = ConstraintMatrixForBilevel(n, Q, R, cc, d,p)


[Adj, ~] = BRandom(n,p,0);
AA = Adj - triu(Adj);
bg = biograph(AA,[],'ShowArrows','off','ShowWeights','off');
h = view(bg); 

MC = maximalCliques(Adj,'v2');
[~,bb] = size(MC);

CliqueConstraint = zeros(bb, n);

for i = 1:bb
    CliqueConstraint(i,:) = -MC(:,i);
end
spy(CliqueConstraint)
%Find a few cliques here and add their constraints
%Then output constraint matrix...

f = zeros(n,1); %x
ff = zeros(1,n); %z


for i = 1:n
    f(i) = -floor(cc(i));
    ff(i) = floor(d(i));
end
    
f1 = [zeros(1,1+2*n+2*bb) ff];
f2 = [zeros(n,1+n+2*bb) eye(n) eye(n)];
f3 = [f -eye(n) zeros(n,bb) MC zeros(n) zeros(n)];
f4 = [zeros(bb,1+n) -eye(bb) -eye(bb) zeros(bb,2*n)];
A = [f1;f2;f3;f4];
AA = [eye(n+1) zeros(n+1, n+bb); zeros(n+bb,n+1) eye(n+bb)];
A = [A AA];

b = [Q; ones(n,1); zeros(n,1); ones(bb,1)];

c = [R; ones(n+bb,1); zeros(n+bb,1); -ones(n,1); zeros(2*n+bb+1,1)];
size(A)
cliques = bb;

end

