/**
 * =============================================================================
 * WEEK 18: MACHINE LEARNING FUNDAMENTALS IN C
 * Homework 2 Solution: Neural Network from Scratch
 * =============================================================================
 *
 * COMPLETE SOLUTION - For Instructor Use Only
 *
 * This solution demonstrates:
 *   1. Configurable multi-layer neural network
 *   2. Forward propagation
 *   3. Backpropagation with gradient descent
 *   4. Sigmoid and ReLU activation functions
 *   5. Training on XOR and digit classification
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c -lm
 * Usage: ./homework2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_LAYERS 5
#define MAX_NEURONS 128
#define MAX_SAMPLES 2000

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================
 */

typedef enum {
    ACTIVATION_SIGMOID,
    ACTIVATION_RELU,
    ACTIVATION_SOFTMAX
} ActivationType;

typedef struct {
    int input_size;
    int output_size;
    double **weights;       /* [output_size][input_size] */
    double *biases;         /* [output_size] */
    double **weight_grads;  /* Gradients for weights */
    double *bias_grads;     /* Gradients for biases */
    ActivationType activation;
} Layer;

typedef struct {
    Layer *layers;
    int num_layers;
    double learning_rate;
    
    /* Intermediate values for backprop */
    double **layer_inputs;   /* Input to each layer */
    double **layer_outputs;  /* Output from each layer */
    double **deltas;         /* Error deltas */
} NeuralNetwork;

typedef struct {
    double *features;
    double *targets;        /* One-hot encoded for classification */
    int label;
} Sample;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

static double random_uniform(double min, double max) {
    return min + (max - min) * ((double)rand() / RAND_MAX);
}

/* Xavier initialisation for better convergence */
static double xavier_init(int fan_in, int fan_out) {
    double limit = sqrt(6.0 / (fan_in + fan_out));
    return random_uniform(-limit, limit);
}

/* =============================================================================
 * ACTIVATION FUNCTIONS
 * =============================================================================
 */

static double sigmoid(double x) {
    if (x > 500) return 1.0;
    if (x < -500) return 0.0;
    return 1.0 / (1.0 + exp(-x));
}

static double sigmoid_derivative(double output) {
    return output * (1.0 - output);
}

static double relu(double x) {
    return (x > 0) ? x : 0;
}

static double relu_derivative(double x) {
    return (x > 0) ? 1.0 : 0.0;
}

static void softmax(double *input, double *output, int n) {
    double max_val = input[0];
    for (int i = 1; i < n; i++) {
        if (input[i] > max_val) max_val = input[i];
    }
    
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        output[i] = exp(input[i] - max_val);
        sum += output[i];
    }
    
    for (int i = 0; i < n; i++) {
        output[i] /= sum;
    }
}

/* =============================================================================
 * LAYER FUNCTIONS
 * =============================================================================
 */

static void layer_init(Layer *layer, int input_size, int output_size,
                       ActivationType activation) {
    layer->input_size = input_size;
    layer->output_size = output_size;
    layer->activation = activation;
    
    /* Allocate weights */
    layer->weights = malloc(output_size * sizeof(double *));
    layer->weight_grads = malloc(output_size * sizeof(double *));
    for (int i = 0; i < output_size; i++) {
        layer->weights[i] = malloc(input_size * sizeof(double));
        layer->weight_grads[i] = calloc(input_size, sizeof(double));
        
        /* Xavier initialisation */
        for (int j = 0; j < input_size; j++) {
            layer->weights[i][j] = xavier_init(input_size, output_size);
        }
    }
    
    /* Allocate biases */
    layer->biases = calloc(output_size, sizeof(double));
    layer->bias_grads = calloc(output_size, sizeof(double));
}

static void layer_free(Layer *layer) {
    for (int i = 0; i < layer->output_size; i++) {
        free(layer->weights[i]);
        free(layer->weight_grads[i]);
    }
    free(layer->weights);
    free(layer->weight_grads);
    free(layer->biases);
    free(layer->bias_grads);
}

static void layer_forward(Layer *layer, double *input, double *output) {
    for (int i = 0; i < layer->output_size; i++) {
        double sum = layer->biases[i];
        for (int j = 0; j < layer->input_size; j++) {
            sum += layer->weights[i][j] * input[j];
        }
        
        /* Apply activation */
        switch (layer->activation) {
            case ACTIVATION_SIGMOID:
                output[i] = sigmoid(sum);
                break;
            case ACTIVATION_RELU:
                output[i] = relu(sum);
                break;
            case ACTIVATION_SOFTMAX:
                output[i] = sum;  /* Softmax applied later */
                break;
        }
    }
    
    /* Apply softmax if needed */
    if (layer->activation == ACTIVATION_SOFTMAX) {
        double temp[MAX_NEURONS];
        memcpy(temp, output, layer->output_size * sizeof(double));
        softmax(temp, output, layer->output_size);
    }
}

/* =============================================================================
 * NEURAL NETWORK FUNCTIONS
 * =============================================================================
 */

static void nn_init(NeuralNetwork *nn, int *layer_sizes, ActivationType *activations,
                    int num_layers, double learning_rate) {
    nn->num_layers = num_layers - 1;  /* Number of weight layers */
    nn->learning_rate = learning_rate;
    nn->layers = malloc(nn->num_layers * sizeof(Layer));
    
    for (int i = 0; i < nn->num_layers; i++) {
        layer_init(&nn->layers[i], layer_sizes[i], layer_sizes[i + 1], activations[i]);
    }
    
    /* Allocate intermediate storage */
    nn->layer_inputs = malloc(nn->num_layers * sizeof(double *));
    nn->layer_outputs = malloc(nn->num_layers * sizeof(double *));
    nn->deltas = malloc(nn->num_layers * sizeof(double *));
    
    for (int i = 0; i < nn->num_layers; i++) {
        nn->layer_inputs[i] = malloc(nn->layers[i].input_size * sizeof(double));
        nn->layer_outputs[i] = malloc(nn->layers[i].output_size * sizeof(double));
        nn->deltas[i] = malloc(nn->layers[i].output_size * sizeof(double));
    }
}

static void nn_free(NeuralNetwork *nn) {
    for (int i = 0; i < nn->num_layers; i++) {
        layer_free(&nn->layers[i]);
        free(nn->layer_inputs[i]);
        free(nn->layer_outputs[i]);
        free(nn->deltas[i]);
    }
    free(nn->layers);
    free(nn->layer_inputs);
    free(nn->layer_outputs);
    free(nn->deltas);
}

static void nn_forward(NeuralNetwork *nn, double *input, double *output) {
    double *current_input = input;
    
    for (int i = 0; i < nn->num_layers; i++) {
        memcpy(nn->layer_inputs[i], current_input, 
               nn->layers[i].input_size * sizeof(double));
        layer_forward(&nn->layers[i], current_input, nn->layer_outputs[i]);
        current_input = nn->layer_outputs[i];
    }
    
    /* Copy final output */
    int final_size = nn->layers[nn->num_layers - 1].output_size;
    memcpy(output, nn->layer_outputs[nn->num_layers - 1], final_size * sizeof(double));
}

static double nn_backward(NeuralNetwork *nn, double *target) {
    /* Compute loss (cross-entropy for classification) */
    double loss = 0.0;
    int final_layer = nn->num_layers - 1;
    int output_size = nn->layers[final_layer].output_size;
    
    for (int i = 0; i < output_size; i++) {
        double pred = nn->layer_outputs[final_layer][i];
        pred = fmax(fmin(pred, 1.0 - 1e-15), 1e-15);  /* Clip for stability */
        loss -= target[i] * log(pred);
    }
    
    /* Output layer delta */
    for (int i = 0; i < output_size; i++) {
        if (nn->layers[final_layer].activation == ACTIVATION_SOFTMAX) {
            /* Cross-entropy + softmax gradient simplifies */
            nn->deltas[final_layer][i] = 
                nn->layer_outputs[final_layer][i] - target[i];
        } else {
            double output = nn->layer_outputs[final_layer][i];
            nn->deltas[final_layer][i] = 
                (output - target[i]) * sigmoid_derivative(output);
        }
    }
    
    /* Backpropagate through hidden layers */
    for (int l = final_layer - 1; l >= 0; l--) {
        Layer *next_layer = &nn->layers[l + 1];
        Layer *curr_layer = &nn->layers[l];
        
        for (int i = 0; i < curr_layer->output_size; i++) {
            double sum = 0.0;
            for (int j = 0; j < next_layer->output_size; j++) {
                sum += next_layer->weights[j][i] * nn->deltas[l + 1][j];
            }
            
            double output = nn->layer_outputs[l][i];
            if (curr_layer->activation == ACTIVATION_SIGMOID) {
                nn->deltas[l][i] = sum * sigmoid_derivative(output);
            } else if (curr_layer->activation == ACTIVATION_RELU) {
                nn->deltas[l][i] = sum * relu_derivative(nn->layer_inputs[l][i]);
            }
        }
    }
    
    /* Accumulate gradients */
    for (int l = 0; l < nn->num_layers; l++) {
        Layer *layer = &nn->layers[l];
        
        for (int i = 0; i < layer->output_size; i++) {
            for (int j = 0; j < layer->input_size; j++) {
                layer->weight_grads[i][j] += nn->deltas[l][i] * nn->layer_inputs[l][j];
            }
            layer->bias_grads[i] += nn->deltas[l][i];
        }
    }
    
    return loss;
}

static void nn_update_weights(NeuralNetwork *nn, int batch_size) {
    for (int l = 0; l < nn->num_layers; l++) {
        Layer *layer = &nn->layers[l];
        
        for (int i = 0; i < layer->output_size; i++) {
            for (int j = 0; j < layer->input_size; j++) {
                layer->weights[i][j] -= 
                    nn->learning_rate * layer->weight_grads[i][j] / batch_size;
                layer->weight_grads[i][j] = 0.0;
            }
            layer->biases[i] -= 
                nn->learning_rate * layer->bias_grads[i] / batch_size;
            layer->bias_grads[i] = 0.0;
        }
    }
}

static void nn_train(NeuralNetwork *nn, Sample *samples, int n_samples,
                     int epochs, int batch_size) {
    double *output = malloc(MAX_NEURONS * sizeof(double));
    
    printf("Training neural network...\n");
    printf("Samples: %d, Epochs: %d, Batch size: %d, LR: %.4f\n\n",
           n_samples, epochs, batch_size, nn->learning_rate);
    
    for (int epoch = 0; epoch < epochs; epoch++) {
        double total_loss = 0.0;
        int correct = 0;
        
        /* Shuffle samples */
        for (int i = n_samples - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Sample temp = samples[i];
            samples[i] = samples[j];
            samples[j] = temp;
        }
        
        for (int batch_start = 0; batch_start < n_samples; batch_start += batch_size) {
            int batch_end = batch_start + batch_size;
            if (batch_end > n_samples) batch_end = n_samples;
            int actual_batch = batch_end - batch_start;
            
            for (int i = batch_start; i < batch_end; i++) {
                nn_forward(nn, samples[i].features, output);
                total_loss += nn_backward(nn, samples[i].targets);
                
                /* Check accuracy */
                int pred_class = 0;
                double max_prob = output[0];
                int output_size = nn->layers[nn->num_layers - 1].output_size;
                
                for (int c = 1; c < output_size; c++) {
                    if (output[c] > max_prob) {
                        max_prob = output[c];
                        pred_class = c;
                    }
                }
                
                if (pred_class == samples[i].label) {
                    correct++;
                }
            }
            
            nn_update_weights(nn, actual_batch);
        }
        
        if (epoch % 100 == 0 || epoch == epochs - 1) {
            double accuracy = 100.0 * correct / n_samples;
            printf("  Epoch %4d: Loss = %.4f, Accuracy = %.2f%%\n",
                   epoch, total_loss / n_samples, accuracy);
        }
    }
    
    free(output);
}

static double nn_evaluate(NeuralNetwork *nn, Sample *samples, int n_samples) {
    double *output = malloc(MAX_NEURONS * sizeof(double));
    int correct = 0;
    
    int output_size = nn->layers[nn->num_layers - 1].output_size;
    
    for (int i = 0; i < n_samples; i++) {
        nn_forward(nn, samples[i].features, output);
        
        int pred_class = 0;
        double max_prob = output[0];
        for (int c = 1; c < output_size; c++) {
            if (output[c] > max_prob) {
                max_prob = output[c];
                pred_class = c;
            }
        }
        
        if (pred_class == samples[i].label) {
            correct++;
        }
    }
    
    free(output);
    return 100.0 * correct / n_samples;
}

/* =============================================================================
 * DATA GENERATION
 * =============================================================================
 */

/**
 * Creates XOR dataset
 */
static void create_xor_dataset(Sample *samples, int *n_samples) {
    double xor_inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    int xor_labels[4] = {0, 1, 1, 0};
    
    *n_samples = 0;
    
    /* Create multiple copies with slight noise for better training */
    for (int rep = 0; rep < 100; rep++) {
        for (int i = 0; i < 4; i++) {
            samples[*n_samples].features = malloc(2 * sizeof(double));
            samples[*n_samples].targets = malloc(2 * sizeof(double));
            
            /* Add small noise */
            samples[*n_samples].features[0] = xor_inputs[i][0] + 
                ((double)rand() / RAND_MAX - 0.5) * 0.1;
            samples[*n_samples].features[1] = xor_inputs[i][1] + 
                ((double)rand() / RAND_MAX - 0.5) * 0.1;
            
            /* One-hot encode */
            samples[*n_samples].targets[0] = (xor_labels[i] == 0) ? 1.0 : 0.0;
            samples[*n_samples].targets[1] = (xor_labels[i] == 1) ? 1.0 : 0.0;
            samples[*n_samples].label = xor_labels[i];
            
            (*n_samples)++;
        }
    }
}

/**
 * Creates synthetic digit dataset (simplified 4x4 patterns)
 */
static void create_digit_dataset(Sample *samples, int *n_samples) {
    /* Simplified 4x4 digit patterns for digits 0-9 */
    int digit_patterns[10][16] = {
        {0,1,1,0, 1,0,0,1, 1,0,0,1, 0,1,1,0},  /* 0 */
        {0,1,1,0, 0,0,1,0, 0,0,1,0, 0,1,1,1},  /* 1 */
        {1,1,1,0, 0,0,1,0, 0,1,0,0, 1,1,1,1},  /* 2 */
        {1,1,1,0, 0,1,1,0, 0,0,1,0, 1,1,1,0},  /* 3 */
        {1,0,1,0, 1,0,1,0, 1,1,1,1, 0,0,1,0},  /* 4 */
        {1,1,1,1, 1,1,0,0, 0,0,1,1, 1,1,1,0},  /* 5 */
        {0,1,1,0, 1,0,0,0, 1,1,1,0, 0,1,1,0},  /* 6 */
        {1,1,1,1, 0,0,1,0, 0,1,0,0, 1,0,0,0},  /* 7 */
        {0,1,1,0, 1,0,0,1, 0,1,1,0, 0,1,1,0},  /* 8 */
        {0,1,1,0, 1,0,0,1, 0,1,1,1, 0,0,1,0}   /* 9 */
    };
    
    *n_samples = 0;
    
    /* Create noisy copies */
    for (int rep = 0; rep < 100; rep++) {
        for (int digit = 0; digit < 10; digit++) {
            samples[*n_samples].features = malloc(16 * sizeof(double));
            samples[*n_samples].targets = malloc(10 * sizeof(double));
            
            /* Add noise to pattern */
            for (int i = 0; i < 16; i++) {
                samples[*n_samples].features[i] = digit_patterns[digit][i] + 
                    ((double)rand() / RAND_MAX - 0.5) * 0.3;
            }
            
            /* One-hot encode */
            for (int c = 0; c < 10; c++) {
                samples[*n_samples].targets[c] = (c == digit) ? 1.0 : 0.0;
            }
            samples[*n_samples].label = digit;
            
            (*n_samples)++;
        }
    }
}

static void free_samples(Sample *samples, int n_samples) {
    for (int i = 0; i < n_samples; i++) {
        free(samples[i].features);
        free(samples[i].targets);
    }
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 18: MACHINE LEARNING FUNDAMENTALS                    ║\n");
    printf("║     Homework 2 Solution: Neural Network from Scratch          ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    srand(42);
    
    Sample *samples = malloc(MAX_SAMPLES * sizeof(Sample));
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: XOR PROBLEM                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n_xor;
    create_xor_dataset(samples, &n_xor);
    printf("Created XOR dataset with %d samples\n\n", n_xor);
    
    /* Network: 2 -> 8 -> 2 */
    int xor_layers[] = {2, 8, 2};
    ActivationType xor_activations[] = {ACTIVATION_SIGMOID, ACTIVATION_SOFTMAX};
    
    NeuralNetwork xor_nn;
    nn_init(&xor_nn, xor_layers, xor_activations, 3, 0.5);
    
    nn_train(&xor_nn, samples, n_xor, 500, 32);
    
    /* Test on canonical XOR inputs */
    printf("\nXOR Test Results:\n");
    printf("┌─────────┬─────────┬──────────┬──────────┐\n");
    printf("│ Input A │ Input B │ Expected │ Output   │\n");
    printf("├─────────┼─────────┼──────────┼──────────┤\n");
    
    double test_inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    int expected[4] = {0, 1, 1, 0};
    double output[2];
    
    for (int i = 0; i < 4; i++) {
        nn_forward(&xor_nn, test_inputs[i], output);
        int pred = (output[1] > output[0]) ? 1 : 0;
        printf("│ %7.0f │ %7.0f │ %8d │ %8d │ %s\n",
               test_inputs[i][0], test_inputs[i][1], expected[i], pred,
               (pred == expected[i]) ? "✓" : "✗");
    }
    printf("└─────────┴─────────┴──────────┴──────────┘\n");
    
    free_samples(samples, n_xor);
    nn_free(&xor_nn);
    
    /* ─────────────────────────────────────────────────────────────────────── */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: DIGIT CLASSIFICATION                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int n_digits;
    create_digit_dataset(samples, &n_digits);
    printf("Created digit dataset with %d samples (4x4 patterns, 10 classes)\n\n", n_digits);
    
    /* Split into train/test */
    int n_train = (int)(n_digits * 0.8);
    int n_test = n_digits - n_train;
    
    Sample *train_samples = samples;
    Sample *test_samples = samples + n_train;
    
    /* Shuffle */
    for (int i = n_digits - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Sample temp = samples[i];
        samples[i] = samples[j];
        samples[j] = temp;
    }
    
    printf("Train: %d samples, Test: %d samples\n\n", n_train, n_test);
    
    /* Network: 16 -> 32 -> 16 -> 10 */
    int digit_layers[] = {16, 32, 16, 10};
    ActivationType digit_activations[] = {ACTIVATION_RELU, ACTIVATION_RELU, ACTIVATION_SOFTMAX};
    
    NeuralNetwork digit_nn;
    nn_init(&digit_nn, digit_layers, digit_activations, 4, 0.1);
    
    nn_train(&digit_nn, train_samples, n_train, 500, 64);
    
    double train_acc = nn_evaluate(&digit_nn, train_samples, n_train);
    double test_acc = nn_evaluate(&digit_nn, test_samples, n_test);
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      FINAL RESULTS                                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Digit Classification Results:\n");
    printf("┌────────────────────┬────────────────┐\n");
    printf("│ Metric             │ Value          │\n");
    printf("├────────────────────┼────────────────┤\n");
    printf("│ Training Accuracy  │ %13.2f%% │\n", train_acc);
    printf("│ Test Accuracy      │ %13.2f%% │\n", test_acc);
    printf("└────────────────────┴────────────────┘\n\n");
    
    if (test_acc >= 90.0) {
        printf("✓ Target accuracy (>90%%) achieved!\n\n");
    } else {
        printf("⚠ Target accuracy (>90%%) not achieved. Try:\n");
        printf("  - More training epochs\n");
        printf("  - Adjusting learning rate\n");
        printf("  - Different network architecture\n\n");
    }
    
    /* Sample predictions */
    printf("Sample Predictions:\n");
    printf("┌───────┬──────────┬───────────┐\n");
    printf("│ Digit │ Actual   │ Predicted │\n");
    printf("├───────┼──────────┼───────────┤\n");
    
    double pred_output[10];
    for (int i = 0; i < 10 && i < n_test; i++) {
        nn_forward(&digit_nn, test_samples[i].features, pred_output);
        
        int pred = 0;
        double max_prob = pred_output[0];
        for (int c = 1; c < 10; c++) {
            if (pred_output[c] > max_prob) {
                max_prob = pred_output[c];
                pred = c;
            }
        }
        
        printf("│ %5d │ %8d │ %9d │ %s\n",
               i, test_samples[i].label, pred,
               (pred == test_samples[i].label) ? "✓" : "✗");
    }
    printf("└───────┴──────────┴───────────┘\n");
    
    /* Network architecture summary */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      NETWORK ARCHITECTURE                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Input Layer:  16 neurons (4×4 image)\n");
    printf("Hidden 1:     32 neurons (ReLU)\n");
    printf("Hidden 2:     16 neurons (ReLU)\n");
    printf("Output:       10 neurons (Softmax)\n\n");
    
    int total_params = 0;
    for (int l = 0; l < digit_nn.num_layers; l++) {
        int params = digit_nn.layers[l].input_size * digit_nn.layers[l].output_size +
                     digit_nn.layers[l].output_size;
        total_params += params;
        printf("Layer %d: %d × %d + %d = %d parameters\n",
               l + 1, digit_nn.layers[l].input_size, digit_nn.layers[l].output_size,
               digit_nn.layers[l].output_size, params);
    }
    printf("Total: %d parameters\n", total_params);
    
    free_samples(samples, n_digits);
    nn_free(&digit_nn);
    free(samples);
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    HOMEWORK 2 COMPLETE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
