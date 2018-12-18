clc
clear all

rgbImage = imread('C:\Users\Junhui Law\Desktop\demo.jpg');
[r,g,b]= imsplit(rgbImage);

%Binarize 2D grayscale image by thresholding, Otsu's threshold
[counts,x]=imhist(g,16);
T=otsuthresh(counts);
BW1 = imbinarize(g,T);

%Find Canny edge
BW2 = edge(BW1,'Canny');

%Turn binary into 8
J = im2uint8(BW2);

%Divide a pixel into 5 pixels
[m,n]= size(J);
A = zeros(m*5,n*5);
for i=1:m
    for j=1:n
        value = J(i,j);
        A(5*i,5*j)=value;
    end
end     

%2D Gaussian filtering
B = imgaussfilt(A,1);

image(B);