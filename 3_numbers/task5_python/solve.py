import sys
 
def gcd (a, b): 
    if a == 0:
        x = 0; y = 1;
        return x, y, b
    
    x1, y1, d = gcd (b%a, a);
    x = y1 - (b // a) * x1;
    y = x1;
    return x, y, d
 
 
def find_any_solution (a, b, c):
    x0, y0, g = gcd (abs(a), abs(b))
    if c % g != 0:
        return -1
    x0 *= c // g
    y0 *= c // g
    if a < 0:   x0 *= -1;
    if b < 0:   y0 *= -1;
    return x0, y0
 
def main():
    N = int(input())
    for _ in range(N):
        a, b, n, m = [int(i) for i in sys.stdin.readline().split()]
        k1, k2 = find_any_solution(n,m,b-a)
        ans = k1*n + a
        nums = abs(ans) // (m*n)
        if ans > 0:
            ans -= (nums)*m*n
        if ans < 0:
            ans += (nums+1)*m*n
        
        print(ans)
    return
 
if __name__ == '__main__':
    main()
