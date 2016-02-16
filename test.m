addpath('../../segbench/Detectors');
addpath('../../segbench/Filters');
addpath('../../segbench/Util');

addpath('../superpixel');

img_id = 55;

% Load RGB image and foreground mask
im = imread(strcat('/home/twang/dataset/lab2/rgb_img_',num2str(img_id),'.png'));
imfore = imread(strcat('/home/twang/dataset/lab2/fore_img_',num2str(img_id),'.png'));
im = imresize(im, [480 640]);
imfore = imresize(imfore, [480 640]);
imfore = (imfore ~= 0);

% Corp out ROI
side = 10; % how many pixels to left out at each side
imfore_nzidx = find(imfore); % nzidx for non-zero index
[imfore_nzidx_i, imfore_nzidx_j] = ind2sub(size(imfore),imfore_nzidx);
min_i = min(imfore_nzidx_i) - side;
max_i = max(imfore_nzidx_i) + side;
min_j = min(imfore_nzidx_j) - side;
max_j = max(imfore_nzidx_j) + side;
im = imcrop(im, [min_j, min_i, max_j-min_j, max_i-min_i]);


% Run Canny edge detector
im1 = im ./ 255;
pb = pbCanny(im1);
subplot(2,2,1);imagesc(pb); axis equal; axis off;

% Compute & Show superpixels
img_sup = computeSupix(im, pb);
img_sup_show = segImage(im, img_sup);
subplot(2,2,2);imagesc(img_sup_show); axis equal; axis off;

% Load depth image
imd = imread(strcat('/home/twang/dataset/lab2/depth_img_',num2str(img_id),'.png'));
imd = imcrop(imd, [min_j, min_i, max_j-min_j, max_i-min_i]);
imd_mask = (imd ~= 0);
subplot(2,2,3);imagesc(imd_mask); axis equal; axis off;

% Fill in missing values
imd_fill = double(imd);
imd_fill = medianfillmissing(imd_fill);
subplot(2,2,4);imagesc(imd_fill); axis equal; axis off;