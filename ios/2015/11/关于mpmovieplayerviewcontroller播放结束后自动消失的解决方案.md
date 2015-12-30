# 关于MPMoviePlayerViewController播放结束后自动消失的解决方案

***

你可以使用这段代码来阻止控制器播放在播放结束后自动dismissing（消失），并且捕捉到用户点击完成的按钮事件去自己定义并处理让你的MPMoviePlayerViewController播放器的消失（dismiss）的时机

***

步骤 1. - 创建并初始化一个MPMoviePlayerViewController（videoPlayer）

```objc
MPMoviePlayerViewController *videoPlayer = [[MPMoviePlayerViewController alloc] initWithContentURL:[[NSURL alloc ]initWithString:[aURL];
```
步骤 2. - 移除videoPlayer默认的通知事件并且加入自己的通知事件。
```objc
[[NSNotificationCenter defaultCenter] removeObserver:videoPlayer
name:MPMoviePlayerPlaybackDidFinishNotification object:videoPlayer.moviePlayer];
[[NSNotificationCenter defaultCenter] addObserver:self
selector:@selector(videoFinished:) name:MPMoviePlayerPlaybackDidFinishNotification object:videoPlayer.moviePlayer];
```
步骤 3. - 手动显示你的videoPlayer控制器
```objc
[self presentMoviePlayerViewControllerAnimated:videoPlayer];
```
步骤 4. - 添加 videoFinish: 方法处理通知事件

```objc
-(void)videoFinished:(NSNotification*)aNotification{
    int value = [[aNotification.userInfo valueForKey:MPMoviePlayerPlaybackDidFinishReasonUserInfoKey] intValue];
    if (value == MPMovieFinishReasonUserExited) {
        [self dismissMoviePlayerViewControllerAnimated];
    }
}
```
***



参考:[http://stackoverflow.com/a/19596598](http://stackoverflow.com/a/19596598)