import os
import pathlib
import subprocess
import cv2
import numpy as np
import tensorflow.compat.v1 as tf
import sys
import matplotlib.pyplot as plt
from PIL import Image

# Import utilites
from object_detection.utils import label_map_util
from object_detection.utils import visualization_utils as vis_util

def prepare():
    # Clone the tensorflow models repository if it doesn't already exist
    if "models" in pathlib.Path.cwd().parts:
      while "models" in pathlib.Path.cwd().parts:
        os.chdir('..')
    elif not pathlib.Path('models').exists():
        command = 'git clone --depth 1 https://github.com/tensorflow/models'
        process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()

    # This is needed since the notebook is stored in the object_detection folder.
    sys.path.append("..")

def load_model():
    # Grab path to current working directory
    CWD_PATH = os.getcwd()

    # Path to frozen detection graph .pb file, which contains the model that is used
    # for object detection.
    PATH_TO_CKPT = '../weights/frozen_inference_graph.pb'

    # Path to label map file
    PATH_TO_LABELS = '../weights/label_map.pbtxt'

    # Path to image
    PATH_TO_IMAGE_FOLDER = 'images/'

    # Number of classes the object detector can identify
    NUM_CLASSES = 1

    # Load the label map.
    # Label maps map indices to category names, so that when our convolution
    # network predicts `5`, we know that this corresponds to `king`.
    # Here we use internal utility functions, but anything that returns a
    # dictionary mapping integers to appropriate string labels would be fine
    category_index = label_map_util.create_category_index_from_labelmap(PATH_TO_LABELS, use_display_name=True)

    # Load the Tensorflow model into memory.
    detection_graph = tf.Graph()
    with detection_graph.as_default():
        od_graph_def = tf.GraphDef()
        with tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
            serialized_graph = fid.read()
            od_graph_def.ParseFromString(serialized_graph)
            tf.import_graph_def(od_graph_def, name='')

        sess = tf.Session(graph=detection_graph)

    # Define input and output tensors (i.e. data) for the object detection classifier

    # Input tensor is the image
    image_tensor = detection_graph.get_tensor_by_name('image_tensor:0')

    # Output tensors are the detection boxes, scores, and classes
    # Each box represents a part of the image where a particular object was detected
    detection_boxes = detection_graph.get_tensor_by_name('detection_boxes:0')

    # Each score represents level of confidence for each of the objects.
    # The score is shown on the result image, together with the class label.
    detection_scores = detection_graph.get_tensor_by_name('detection_scores:0')
    detection_classes = detection_graph.get_tensor_by_name('detection_classes:0')

    # Number of objects detected
    num_detections = detection_graph.get_tensor_by_name('num_detections:0')

    args = [detection_boxes, detection_scores, detection_classes, num_detections]

    return sess, args, image_tensor

def loadImage(path, convert):
    image = cv2.imread(path)
    if convert:
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    return image

def classify(image, sess, args, image_tensor):
    image_expanded = np.expand_dims(image, axis=0)


    # Perform the actual detection by running the model with the image as input
    (boxes, scores, classes, num) = sess.run(args,feed_dict={image_tensor: image_expanded})

    coords = reduceBoxes(boxes[0], scores[0], classes)
    ranges = [findRange(coords[i]) for i in range(len(coords))]
    return sum(ranges) / len(ranges) if len(ranges) else None

def findRange(box):
    # Coords in {TopDist, LeftDist, BotDist, RightDist}
    # Returns a value between -1 and 1 for left and right
    leftDist = box['box'][1]
    rightDist = box['box'][3]
    avg = (rightDist - leftDist) / 2 + (leftDist)
    return avg * 2 - 1

def reduceBoxes(boxes, scores, classes):
    '''Function to reduce all the boxes to only the ones being displayed.
    Slightly modified version of Shreyas Vedpathak's version from stackoverflow:
    https://stackoverflow.com/a/68376832/19442761'''

    max_boxes_to_draw = boxes.shape[0]
    min_score_thresh=.5

    # Loop over all objects
    coordinates = []
    for i in range(min(max_boxes_to_draw, boxes.shape[0])):
        if scores[i] > min_score_thresh:
            class_id = int(classes[0][i] + 1)
            coordinates.append({
                "box": boxes[i],
                "score": scores[i]
            })

    return coordinates

def cropImg(image, box):
    # box in {TopDist, LeftDist, BotDist, RightDist}, in ratios
    # left, upper, right, and lower
    PIL_image = Image.fromarray(np.uint8(image)).convert('RGB')
    siz = PIL_image.size
    hwhw = np.tile(siz, 2).flatten()

    # Cropping as left, upper, right, and lower
    newbox = np.array([box[1], box[0], box[3], box[2]])
    imgcoords = hwhw * newbox

    return PIL_image.crop(imgcoords)

def setup():
    prepare()
    return load_model()

# sess, args, feed_dict = setup()
# classify(loadImage("images/test.jpg"), sess, args, feed_dict)
