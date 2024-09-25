import cv2 as cv
import numpy as np

IMG_PATH = "./image.png"

def detect_indicator(img):
    cimg = cv.cvtColor(img,cv.COLOR_GRAY2RGB)
    ret,img = cv.threshold(img,250,255,cv.THRESH_BINARY)

    kernel = cv.getStructuringElement(cv.MORPH_ELLIPSE,(3,3))
    img = cv.morphologyEx(img, cv.MORPH_CLOSE, kernel,iterations = 5) #reduce holes
    img = cv.dilate(img,kernel,iterations = 5) # expand size
    # contours, hierarchy = cv.findContours(img,cv.RETR_TREE, cv.CHAIN_APPROX_NONE)
    contours, hierarchy = cv.findContours(img,cv.RETR_CCOMP, cv.CHAIN_APPROX_NONE)
    # print("num of contours:",len(contours))
    # cv.imshow("binarized_img",img)
    
    # Approximate contours to polygons + get bounding rects and circles
    contours_poly = [None]*len(contours)
    boundRect = [None]*len(contours)
    centers = [None]*len(contours)
    radius = [None]*len(contours)
    for i, c in enumerate(contours):
        # contours_poly[i] = cv.approxPolyDP(c, 3, True)
        # boundRect[i] = cv.boundingRect(contours_poly[i])
        # centers[i], radius[i] = cv.minEnclosingCircle(contours_poly[i])
        boundRect[i] = cv.boundingRect(c)
        centers[i], radius[i] = cv.minEnclosingCircle(c)
    
    cv.drawContours(cimg, contours, -1, (0, 255, 0), 1)
    # cv.drawContours(cimg, contours, 0, (0, 255, 0), 3)
    boundingRect = cv.boundingRect(contours[0])
    # print("boundingRect:",boundingRect) #x-coor, y-coor, width, height
    cv.rectangle(cimg, (int(boundingRect[0]), int(boundingRect[1])), \
          (int(boundingRect[0]+boundingRect[2]), int(boundingRect[1]+boundingRect[3])), (255,0,0), 1)
    # cv.imshow("indicator", cimg)
    # cv.imwrite("indicator.png", cimg)
    
    return boundingRect
    


if __name__ == "__main__":

    # read image
    ori_img = cv.imread(IMG_PATH,cv.IMREAD_COLOR)
    ori_img_gray = cv.cvtColor(ori_img,cv.COLOR_BGR2GRAY)

    
    padding_size = 10
    padded_image = cv.copyMakeBorder(
        ori_img,
        top=padding_size,
        bottom=padding_size,
        left=padding_size,
        right=padding_size,
        borderType=cv.BORDER_CONSTANT,
        value=(255, 255, 255)  # White color in BGR
    )

    img = cv.cvtColor(padded_image,cv.COLOR_BGR2GRAY)
    print("img shape:",img.shape) # height, width
    imgCenterY = img.shape[0]//2
    imgCenterX = img.shape[1]//2
    print("imgCenterX", imgCenterX)
    print("imgCenterY", imgCenterY)
    
    # show img
    boundingRect = detect_indicator(img) #x-coor, y-coor, width, height

    boundingRectCenterX = boundingRect[0] + (boundingRect[2]//2)
    boundingRectCenterY = boundingRect[1] + (boundingRect[3]//2)
    print("boundingRect", boundingRect)
    print("boundingRectCenterX", boundingRectCenterX)
    print("boundingRectCenterY", boundingRectCenterY)

    # rotate img
    res_img = ori_img_gray.copy()
    #1st quardrant no need rotate
    if (boundingRectCenterX > imgCenterX and boundingRectCenterY < imgCenterY): #2st quardrant
        res_img = cv.rotate(res_img,cv.ROTATE_90_COUNTERCLOCKWISE)
        pass
    elif (boundingRectCenterX < imgCenterX and boundingRectCenterY > imgCenterY): #3nd quardrant
        res_img = cv.rotate(res_img,cv.ROTATE_90_CLOCKWISE)
        pass
    elif (boundingRectCenterX > imgCenterX and boundingRectCenterY > imgCenterY): #4th quardrant
        res_img = cv.rotate(res_img,cv.ROTATE_180)
        pass

    hist = cv.calcHist([res_img],[0],None,[256],[0,256])
    # print(hist)
    histSize = 256
    hist_w = 512
    hist_h = 400
    bin_w = int(round( hist_w/histSize ))
    histImage = np.zeros((hist_h, hist_w, 1), dtype=np.uint8)
    cv.normalize(hist, hist, alpha=0, beta=hist_h, norm_type=cv.NORM_MINMAX)
    for i in range(1, histSize):
        cv.line(histImage, ( bin_w*(i-1), hist_h - int(hist[i-1]) ),
                ( bin_w*(i), hist_h - int(hist[i]) ),
                ( 255, 0, 0), thickness=1)
    cv.imshow("Histogram", histImage)

    # cv.imwrite("./output.png",res_img)
    # cv.imshow("Original Image", ori_img)
    cv.imshow("Output Image", res_img)

    # wait for a key press indefinitely
    cv.waitKey(0)

    # close windows
    cv.destroyAllWindows()