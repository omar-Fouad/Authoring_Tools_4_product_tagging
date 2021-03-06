#ifndef FACE_H
#define FACE_H

#include "general.h"

using namespace cv;
using namespace std;

struct Eye{
  Point2f left_pos,right_pos;  
  float left_radius,right_radius;
};

namespace face{
	class Face{
		public:
		  //Face();
          //~Face();

          // Crops and aligns face images for recognition
          void align_face(Mat &out, Mat & img, struct Eye eye, float offset_x, float offset_y, Size sz, string mode = "front");
	};

	class FaceDetect: public Face{
        public:
            // Classifier Parameters
            double FACE_CLASSIFIER_SCALE, EYE_CLASSIFIER_SCALE;
            int MIN_FACE_DETECT_SIZE, MAX_FACE_DETECT_SIZE, MIN_EYE_DETECT_SIZE, MAX_EYE_DETECT_SIZE, EYE_DETECT_FACE_SIZE;
            string eye_classifier_name, face_classifier_name;
            CascadeClassifier eye_classifier, face_classifier;

        public:
            // Constructor initializing all classifier parameters
            FaceDetect():Face(){
                FACE_CLASSIFIER_SCALE = 1.2;    // Reduce image size by this scale for faster detection
                EYE_CLASSIFIER_SCALE = 1.2;     // Enlarge face images to bigger size so that eyes are detected
                MIN_FACE_DETECT_SIZE = 40;      // Minimum bound for face detection
                MAX_FACE_DETECT_SIZE = 300;     // Maximum bound for face detection
                MIN_EYE_DETECT_SIZE = 1;        // Minimum bound for eye detection
                MAX_EYE_DETECT_SIZE = 60;       // Maximum bound for eye detection
                EYE_DETECT_FACE_SIZE = 200;     // Enlarge face images to this size for eye detection

                // Path to classifier xml files
                eye_classifier_name = "../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
                //eye_classifier_name = "../data/haarcascades/haarcascade_lefteye_2splits.xml";
                face_classifier_name = "../data/haarcascades/haarcascade_frontalface_alt.xml";

                if (!face_classifier.load(face_classifier_name))
                  cerr << "Face Detection Classifier cannot be loaded" << endl;
                if (!eye_classifier.load(eye_classifier_name))
                  cerr << "Eye Detection Classifier cannot be loaded" << endl;
            }
			//~FaceDetect();

            // Returns a vector of face bounding boxes and eye circle pairs given the input image
			double detect_face(vector <Rect> &face_pos, vector <struct Eye> &eye, Mat &img, double scale = 1, string mode = "front");

            // Returns a vector of eye circle pairs given face image
			void detect_eyes(struct Eye &eye_pos, Mat &img);

            // Draws a bounding box for the face and a circle for the eyes given the frame and positions
			void draw_face(vector <Rect> &face_pos, vector <struct Eye> & eye_pos, Mat &img, bool eye_flag = false);

            // Draws a circle for the eyes given the positions and image
            void draw_eyes(vector <struct Eye> & eye_pos, Mat &img);
	};

	class FaceRecognize: public FaceDetect{
        public:
            // Face Recognition parameters
            int FACE_RECOGNIZE_SIZE, EYE_LEVEL, EIGEN_THRESH, FISHER_THRESH, LBPH_THRESH;
            vector <string> face_name;
            Ptr<FaceRecognizer> eigen_recognizer, fisher_recognizer, lbph_recognizer;
        public:
            // Constructor initializing Face Recognition parameters
            FaceRecognize():FaceDetect(){
                FACE_RECOGNIZE_SIZE = 70;
                EYE_LEVEL = FACE_RECOGNIZE_SIZE/3;
                EIGEN_THRESH = 1900;
                FISHER_THRESH = 85;
                LBPH_THRESH = 70;
            }
			//~FaceRecognize();

            // Command line way for training a classifier for recognition
			void train(string train_file, string class_file, string recognizer_file);

            // Load a pre-saved classifier for Face Recognition
			void load(string file_recognizer, string file_class);

            // Recognizes and returns labels of detected faces
			void recognize_face(vector <string> &name, vector <double> &confidence, vector <int> &label, vector <Rect> &face_pos, Mat &img, string classifier = "fisher");

            // Labels recognized faces given position, label and frame
			void label_face(Mat &img, vector <string> &label, vector <Rect> &face_pos);
	};
	
	class FaceTracker: public FaceRecognize{
        public:
            int NUM_FRAMES_WITHOUT_DETECTION, KLT_MAX_FEATURES_PER_FACE, KLT_MIN_FEATURES_PER_FACE, ROI_ALLOWANCE;
            double KLT_FEATURE_QUALITY, KLT_MIN_FEATURE_DIST;
            public:
                FaceTracker():FaceRecognize(){
                  NUM_FRAMES_WITHOUT_DETECTION = 10;
                  KLT_MAX_FEATURES_PER_FACE = 20;
                  KLT_MIN_FEATURES_PER_FACE = 10;
                  KLT_FEATURE_QUALITY = 0.01;
                  KLT_MIN_FEATURE_DIST = 4;
                  ROI_ALLOWANCE = 10;
                }
                //~FaceTracker();
                // Tracks face based on optical flow
                void klt_track_face(vector <string> &name, vector <int> &label, vector <Rect> &face_pos, Mat &img, Mat &prev_img, vector < vector <Point2f> > &features, string classifier = "fisher", string mode = "front");
	};
}

#endif
