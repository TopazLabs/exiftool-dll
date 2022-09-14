use warnings;
use Image::ExifTool ':Public';

print("ExifTool loaded!\n");

my $className = 'Image::ExifTool';
my $exifTool = new Image::ExifTool;

my $outFile = 'metadata.exif';
$exifTool->Options(Sort => 'Tag');
$exifTool->SetNewValuesFromFile('test-coach.jpg');
$exifTool->SetNewValue(Artist => 'CQCumbers');
$exifTool->WriteInfo(undef, $outFile, 'EXIF');

my @ioTagList;
my $info = $exifTool->ImageInfo($outFile, \@ioTagList);
for my $tag (@ioTagList) {
    print "$tag : $info->{$tag}\n";
}
