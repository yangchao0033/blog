##***第四章***

### ***IB中得设置图像属性`Attributes Inspector`***

* *IB中得设置图像属性*  

	* `Alpha`设置透明度，需要设置为1.0，防止系统为半透明图层渲染耗费性能 __(非必要时，确保设为1.0)__
	 
	* `Qpaque`同时设置值为YES，防止Alpha通道造成孔洞而多余渲染，告诉视图下方不需要绘制任何图层，减少性能的浪费.勾选后不必在对象视图背后的东西上浪费处理时间了，__(非必要时，确保勾选)__
	
	* `Clears Gtaphics Context` (基本上不用)用来在对象视图绘制之前，使用透明的黑色覆盖所有区域，会损耗性能__(非必要时，确保没有勾选)__
	
	* `Clisp Subviews` 用来确保超出父控件的视图不会被绘制，计算需要裁减区域会耗费性能，所以除非必要，否则__(非必要时，确保没有勾选)__
	
	* `Autoresize SubViews` 通知iOS，当当前视图大小发生变化时，调整子视图的大小__(保留默认值即可)__
	
	* `Stretching(拉伸)` 可以忽略，只有屏幕调整矩形视图大小导致重绘时，才需要拉伸.用于将视图的外边缘（按钮边框）保持不变，仅拉伸中间部分四个浮点值用于指定矩形可拉升区域的左上角坐标及大小**(可忽略)**__<可用于做微信气泡>__
	
* *文本框键盘处理*

	* `Did End On Exit` 当用户点击return键后，会触发此事件，一般用于关闭键盘
	* `点击非响应背景即可关闭键盘` 在故事板中将控制器根view的实现类改为`UIControl`并给view添加`Touch Down`事件，用endEditing注销键盘
	