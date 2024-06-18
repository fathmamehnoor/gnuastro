__kernel void match_arrange(__global float* cat_data,
                             __global int* permutation,
                             int nummatched,
                             int num_columns) {
    int tid = get_global_id(0);

    // Iterate over all columns associated with this thread
    for (int i = 0; i < num_columns; ++i) {
        // For easy reading
        int index = tid;

        // Find the starting index for this column in the cat_data array
        int start_index = index * nummatched;
        int end_index = (index + 1) * nummatched;

        // Rearrange this column's elements
        for (int j = start_index; j < end_index; ++j) {
            int dest_index = j - start_index;
            int src_index = permutation[j];

            // Copy data from source index to destination index
            cat_data[dest_index] = cat_data[src_index];
        }
    }
}
