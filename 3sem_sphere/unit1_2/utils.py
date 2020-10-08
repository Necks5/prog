import numpy as np
from sklearn.tree import DecisionTreeRegressor
from sklearn.ensemble import GradientBoostingRegressor
from sklearn.metrics import mean_squared_error

class DesTreeReg:
    NON_LEAF_TYPE = 0
    LEAF_TYPE = 1
    def __init__(self, min_samples_split=2, 
                 max_depth = 5, max_features = 0.5):
        self.tree = dict()
        self.min_samples_split = min_samples_split
        self.max_depth = max_depth
        self.max_features_percent = max_features
        
    def __find_threshold(self, x, y):
        sorted_idx_x = x.argsort()
        x_sorted = x[sorted_idx_x]
        y_sorted = y[sorted_idx_x]
        
        y_sq_cumsum = np.cumsum(y_sorted ** 2, axis=0)
        y_sq_cumsum_reversed = np.cumsum(y_sorted[::-1] ** 2, axis=0)[::-1]

        y_cumsum = np.cumsum(y_sorted, axis=0)
        y_cumsum_reversed = np.cumsum(y_sorted[::-1], axis=0)[::-1]
    
        cnt_array = np.array(range(1, y_cumsum.shape[0] + 1))#.reshape(-1, 1)
        mse_left = y_sq_cumsum - y_cumsum ** 2 / cnt_array
        mse_right = y_sq_cumsum_reversed - y_cumsum_reversed ** 2 / cnt_array[::-1]
        
        mse = mse_left
        mse[:-1] += mse_right[1:]
        
        mse_argmin = np.argmin(mse)
        
        if mse_argmin == x_sorted.shape[0] - 1:
            return None
        
        threshold =(x_sorted[mse_argmin] + x_sorted[mse_argmin+1]) / 2
        
        return mse[mse_argmin], threshold
    
    def __fit_node(self, x, y, node_id, depth):
        if depth > self.max_depth or y.shape[0] < self.min_samples_split:
            self.tree[node_id] = [self.LEAF_TYPE, np.mean(y)]
            return
        
        feature_ids = np.random.choice(x.shape[1], int(x.shape[1] * self.max_features_percent), replace = False)
        threshold_array = np.array(list(self.__find_threshold(x[:, i], y) for i in feature_ids))
        best_feature_split = np.argmin(threshold_array[:, 0])
        
        left_mask = x[:, feature_ids[best_feature_split]] < threshold_array[best_feature_split][1]
        left_x = x[left_mask]
        right_x = x[~left_mask]
        left_y = y[left_mask]
        right_y = y[~left_mask]
        if left_y.size == 0 or right_y.size == 0:
            self.tree[node_id] = [self.LEAF_TYPE, np.mean(y)]
            return
        self.tree[node_id] = [self.NON_LEAF_TYPE, feature_ids[best_feature_split], 
                              threshold_array[best_feature_split][1]]
        self.__fit_node(left_x, left_y, node_id * 2 + 1, depth + 1)
        self.__fit_node(right_x, right_y, node_id * 2 + 2, depth + 1)
        
    def fit(self, x, y):
        self.__fit_node(x, y, 0, 0)
        
    def __predict_class(self, x, node_id):
        node = self.tree[node_id]
        if node[0] == self.NON_LEAF_TYPE:
            _, feature_id, threshold = node
            if x[feature_id] < threshold:
                return self.__predict_class(x, 2 * node_id + 1)
            else:
                return self.__predict_class(x, 2 * node_id + 2)
        else:
            return node[1]
        
    def predict(self, X):
        return np.array([self.__predict_class(x, 0) for x in X])
    
    def update_leafs(self, learning_rate, b):
        for key in self.tree.keys():
            if self.tree[key][0] == self.LEAF_TYPE:
                self.tree[key][1] *= learning_rate * b
                
class MyGradientBoostingRegressor:
    def __init__(self, learning_rate=0.1, n_estimators=100,
                 min_samples_split=2, max_depth=3, max_features=0.5, 
                subsample=1.0):
        self.learning_rate = learning_rate
        self.n_estimators = n_estimators
        self.min_samples_split = min_samples_split
        self.max_depth = max_depth
        self.max_features = max_features
        self.subsample = subsample
        
        self.estimators = []
        self.mse_train = []
        self.mse_test = []
        
    def fit(self, x, y, x_test = None, y_test = None):
        self.mse_train = []
        self.mse_test = []
        
        temp_tree = DesTreeReg(min_samples_split = self.min_samples_split, 
                              max_depth = self.max_depth,
                              max_features = self.max_features)
        temp_tree.fit(x, y)
        self.estimators.append(temp_tree)
        
        predictions = self.estimators[-1].predict(x)
        
        self.mse_train.append(np.mean((predictions - y) ** 2))
        if x_test is not None:
            predictions_test = np.sum([tree.predict(x_test) for tree in self.estimators], axis=0)
            self.mse_test.append(np.mean((predictions_test - y_test) ** 2))
            
        for est_idx in range(self.n_estimators - 1):
            ids = np.random.choice(x.shape[0], int(x.shape[0] * self.subsample), replace = False)
            gradient = 2.0 * (y[ids] - predictions[ids])
            temp_tree = DesTreeReg(min_samples_split = self.min_samples_split, 
                                  max_depth = self.max_depth,
                                  max_features = self.max_features)
            temp_tree.fit(x[ids], gradient)
            self.estimators.append(temp_tree)
            
            prediction_i = self.estimators[-1].predict(x)
            b_ = ((prediction_i[ids] * (y[ids] - predictions[ids])).sum() /
                     ((prediction_i[ids]**2).sum()))
            
            self.estimators[-1].update_leafs(self.learning_rate, b_)
            
            predictions += self.estimators[-1].predict(x)
            
            self.mse_train.append(np.mean((predictions - y) ** 2))
            if x_test is not None:
                predictions_test = np.sum([tree.predict(x_test) for tree in self.estimators], axis=0)
                self.mse_test.append(np.mean((predictions_test - y_test) ** 2))
                
        self.mse_test = np.array(self.mse_test)
        self.mse_train = np.array(self.mse_train)
            
    def predict(self, x):
        return np.sum((tree.predict(x) for tree in self.estimators))
    
    
    
class BagBoo:
    def __init__(self, base_estimator = "MyGB", n_trees = 5, subsampleBB = 0.5,
                learning_rate=0.1, n_estimators=100,
                 min_samples_split=2, max_depth=3, max_features=0.5, 
                subsampleGB=1.0):
        '''
        base_estimator: MyGB - my GradientBoostingRegressor
                        other - GradientBoostingRegressor from skitlearn
        '''
        self.n_trees = n_trees
        self.base_estimator = base_estimator
        self.subsampleBB = subsampleBB
        self.learning_rate = learning_rate
        self.n_estimators = n_estimators
        self.min_samples_split = min_samples_split
        self.max_depth = max_depth
        self.max_features = max_features
        self.subsampleGB = subsampleGB
        
        self.trees = []
        self.mse_train = []
        self.mse_test = []
        
    def fit(self, x, y, x_test, y_test):
        self.mse_train = []
        self.mse_test = []
        if self.base_estimator == "MyGB":
            for tree_ids in range(self.n_trees):
                print(tree_ids, end = '.', flush=True)
                ids = np.random.choice(x.shape[0], int(x.shape[0] * self.subsampleBB), replace = False)
                
                temp_tree = MyGradientBoostingRegressor(learning_rate=self.learning_rate,
                                n_estimators=self.n_estimators, min_samples_split=self.min_samples_split, 
                                max_depth=self.max_depth, max_features=self.max_features, 
                                subsample=self.subsampleGB)
                temp_tree.fit(x[ids], y[ids], x_test, y_test)
                mse_train_i = np.full(100, np.nan)
                mse_train_i[:min(temp_tree.mse_train.shape[0], 100)] = temp_tree.mse_train
                mse_test_i = np.full(100, np.nan)
                mse_test_i[:min(temp_tree.mse_test.shape[0], 100)] = temp_tree.mse_test
                
                self.mse_train.append(mse_train_i)
                self.mse_test.append(mse_test_i)
                
                self.trees.append(temp_tree)
                
        else:
            for tree_ids in range(self.n_trees):
                print(tree_ids, end = '.', flush=True)
                ids = np.random.choice(x.shape[0], int(x.shape[0] * self.subsampleBB), replace = False)
                model = GradientBoostingRegressor(criterion='mse', init=DecisionTreeRegressor(max_depth = 3),
                                                  learning_rate=self.learning_rate,
                                n_estimators=self.n_estimators, min_samples_split=self.min_samples_split, 
                                max_depth=self.max_depth, max_features=self.max_features, 
                                subsample=self.subsampleGB)
                model.n_estimators = 1
                model.warm_start = True
                errors = []
                for est_num in range(1, self.n_estimators):
                    model.n_estimators = est_num
                    model.fit(x, y)
                    pred = model.predict(x_test)
                    error = mean_squared_error(y_test, pred)
                    errors.append(error)
                    
                self.mse_train.append(model.train_score_)
                self.mse_test.append(np.array(errors))
                
        self.mse_train = np.nanmean(np.array(self.mse_train), axis=0)
        self.mse_test = np.nanmean(np.array(self.mse_test), axis=0)
    
    def predict(self, x, y):
        return np.mean((tree.predict(x) for tree in self.trees))
    
def parse_sparce(path):
    x, y = [], []
    with open(path) as file:
        for idx, line in enumerate(file):
            y_x = line.strip().split(' ')
            temp_y, xs = y_x[0], y_x[1:]
            y.append(temp_y)
            x_line = np.zeros(246)
            for seek, fea in (pair.split(':') for pair in xs):
                x_line[int(seek)] = float(fea)
            x.append(x_line)
    x = np.array(x).astype(np.float32)
    y = np.array(y).astype(np.float32)
    return x, y
    