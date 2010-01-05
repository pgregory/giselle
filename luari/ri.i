%include <typemaps.i>


%module ri
%{
#include "ri.h"
%}

%{
void processParameterList(lua_State* L, int input, RtInt count, RtToken* tokens, RtPointer* values)
{
        int i = 0;
        int arrayI = 0;
        int arrayLen;
        const char* token;
        int valType;
        float* floatValArray;
        char** stringValArray;
        const char* stringVal;
        lua_pushnil(L);  /* first key */
        while (lua_next(L, input) != 0)
        {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                token = lua_tostring(L, -2);
                tokens[i] = SWIG_ALLOC_ARRAY(char,strlen(token)+1);
                strcpy(tokens[i], token);

                /* Now work out the type and process it as appropriate. */
                valType = lua_type(L, -1);
                switch(valType)
                {
                        case LUA_TNUMBER:
                        {
                                floatValArray = SWIG_ALLOC_ARRAY(float, 1);
                                floatValArray[0] = (float)lua_tonumber(L, -1);
                                values[i] = (RtPointer)floatValArray;
                        }
                        break;

                        case LUA_TSTRING:
                        {
                                stringValArray = SWIG_ALLOC_ARRAY(char*, 1);
                                stringVal = lua_tostring(L, -1);
                                stringValArray[0] = SWIG_ALLOC_ARRAY(char, strlen(stringVal));
                                strcpy(stringValArray[0], stringVal);
                                values[i] = (RtPointer)stringValArray;
                        }
                        break;

                        case LUA_TTABLE:
                        {
                                arrayLen = SWIG_itable_size(L, -1);
                                if(arrayLen == 0)
                                {
                                        lua_pushstring(L,"table appears to be empty");
                                        break;
                                }
                                lua_rawgeti(L, -1, 1);
                                valType = lua_type(L, -1);
                                lua_pop(L, 1);
                                switch(valType)
                                {
                                        case LUA_TNUMBER:
                                        {
                                                floatValArray = SWIG_ALLOC_ARRAY(float, arrayLen);
                                                for (arrayI = 0; arrayI < arrayLen; arrayI++)
                                                {
                                                        lua_rawgeti(L,-1,arrayI+1);
                                                        if (lua_isnumber(L,-1))
                                                                floatValArray[arrayI] = (float)lua_tonumber(L,-1);
                                                        else
                                                                lua_pop(L,1);
                                                        lua_pop(L,1);
                                                }
                                                values[i] = (RtPointer)floatValArray;
                                        }
                                        break;

                                        case LUA_TSTRING:
                                        {
                                                stringValArray = SWIG_ALLOC_ARRAY(char*, arrayLen);
                                                for (arrayI = 0; arrayI < arrayLen; arrayI++)
                                                {
                                                        lua_rawgeti(L,-1,arrayI+1);
                                                        if (lua_isstring(L,-1))
                                                        {
                                                                stringVal = lua_tostring(L, -1);
                                                                stringValArray[arrayI] = SWIG_ALLOC_ARRAY(char, strlen(stringVal));
                                                                strcpy(stringValArray[arrayI], stringVal);
                                                        }
                                                        else
                                                                lua_pop(L,1);
                                                        lua_pop(L,1);
                                                }
                                                values[i] = (RtPointer)stringValArray;
                                        }
                                        break;

                                        default:
                                                break;
                                }
                        }
                        break;

                        default:
                                values[i] = 0;
                }
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
                ++i;
        }
}
%}

%typemap(in) (RtInt count, RtToken tokens[], RtPointer values[])
%{	
	$1 = 0;
	$2 = NULL;
	$2 = NULL;
    if (!lua_istable(L,$input))
	{
		lua_pushstring(L,"expected a table");
		return 0;
	}
	$1=SWIG_table_size(L,$input);
    if ($1>=1)
	{
        $2=SWIG_ALLOC_ARRAY(RtToken,$1);
        $3=SWIG_ALLOC_ARRAY(RtPointer,$1);
        processParameterList(L, $input, $1, $2, $3);
    }
%}
%typemap(default) (RtInt count, RtToken tokens[], RtPointer values[])
%{	
	$1 = 0;
	$2 = NULL;
	$3 = NULL;
%}
%typemap(in) RtMatrix
%{
    {
        int r = 0, c = 0;
        if (!lua_istable(L,$input))
        {
                lua_pushstring(L,"expected a table");
                return 0;
        }
        int size = SWIG_table_size(L,$input);
        if (size != 4)
        {
                lua_pushstring(L,"expected 4 entries");
                return 0;
        }

        $1 = SWIG_ALLOC_ARRAY(float[4], 4);
        lua_pushnil(L);
        while(lua_next(L, $input) != 0)
        {
            const char* type = lua_typename(L, lua_type(L, -1));
            if(!lua_istable(L, -1))
            {
                lua_pushstring(L, "ill formed matrix table");
                return 0;
            }
            lua_pushnil(L);
            while(lua_next(L, -2))
            {
                $1[r][c] = lua_tonumber(L, -1);
                lua_pop(L, 1);
                r += 1;
            }
            lua_pop(L, 1);
            c += 1;
            r = 0;
        }
    }
%}
%typemap (freearg) RtMatrix
%{
    SWIG_FREE_ARRAY($1);
%}


%include "ritypes.h"

%rename(Declare) RiDeclare;
RtToken RiDeclare( RtString name, RtString declaration );

%rename(Begin) RiBegin;
RtVoid RiBegin( RtToken name );
%rename(End) RiEnd;
RtVoid RiEnd(  );
%rename(GetContext) RiGetContext;
RtContextHandle RiGetContext(  );
%rename(Context) RiContext;
RtVoid RiContext( RtContextHandle handle );
%rename(FrameBegin) RiFrameBegin;
RtVoid RiFrameBegin( RtInt number );
%rename(FrameEnd) RiFrameEnd;
RtVoid RiFrameEnd(  );
%rename(WorldBegin) RiWorldBegin;
RtVoid RiWorldBegin(  );
%rename(WorldEnd) RiWorldEnd;
RtVoid RiWorldEnd(  );

%rename(IfBegin) RiIfBegin;
RtVoid RiIfBegin( RtString condition );
%rename(ElseIf) RiElseIf;
RtVoid RiElseIf( RtString condition );
%rename(Else) RiElse;
RtVoid RiElse(  );
%rename(IfEnd) RiIfEnd;
RtVoid RiIfEnd(  );

%rename(Format) RiFormat;
RtVoid RiFormat( RtInt xresolution, RtInt yresolution, RtFloat pixelaspectratio );
%rename(FrameAspectRatio) RiFrameAspectRatio;
RtVoid RiFrameAspectRatio( RtFloat frameratio );
%rename(ScreenWindow) RiScreenWindow;
RtVoid RiScreenWindow( RtFloat left, RtFloat right, RtFloat bottom, RtFloat top );
%rename(CropWindow) RiCropWindow;
RtVoid RiCropWindow( RtFloat xmin, RtFloat xmax, RtFloat ymin, RtFloat ymax );
%rename(Projection) RiProjectionV;
RtVoid RiProjectionV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Clipping) RiClipping;
RtVoid RiClipping( RtFloat cnear, RtFloat cfar );
%rename(ClippingPlane) RiClippingPlane;
RtVoid RiClippingPlane( RtFloat x, RtFloat y, RtFloat z, RtFloat nx, RtFloat ny, RtFloat nz );
%rename(DepthOfField) RiDepthOfField;
RtVoid RiDepthOfField( RtFloat fstop, RtFloat focallength, RtFloat focaldistance );
%rename(Shutter) RiShutter;
RtVoid RiShutter( RtFloat opentime, RtFloat closetime );

%rename(PixelVariance) RiPixelVariance;
RtVoid RiPixelVariance( RtFloat variance );
%rename(PixelSamples) RiPixelSamples;
RtVoid RiPixelSamples( RtFloat xsamples, RtFloat ysamples );
%rename(PixelFilter) RiPixelFilter;
RtVoid RiPixelFilter( RtFilterFunc function, RtFloat xwidth, RtFloat ywidth );
%rename(Exposure) RiExposure;
RtVoid RiExposure( RtFloat gain, RtFloat gamma );
%rename(Imager) RiImagerV;
RtVoid RiImagerV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Quantize) RiQuantize;
RtVoid RiQuantize( RtToken type, RtInt one, RtInt min, RtInt max, RtFloat ditheramplitude );
%rename(Display) RiDisplayV;
RtVoid RiDisplayV( RtToken name, RtToken type, RtToken mode, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Hider) RiHiderV;
RtVoid RiHiderV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(ColorSamples) RiColorSamples;
RtVoid RiColorSamples( RtInt N, RtFloat nRGB[], RtFloat RGBn[] );
%rename(RelativeDetail) RiRelativeDetail;
RtVoid RiRelativeDetail( RtFloat relativedetail );

%rename(Option) RiOptionV;
RtVoid RiOptionV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(AttributeBegin) RiAttributeBegin;
RtVoid RiAttributeBegin(  );
%rename(AttributeEnd) RiAttributeEnd;
RtVoid RiAttributeEnd(  );

%rename(Color) RiColor;
RtVoid RiColor( RtColor Cq );
%rename(Opacity) RiOpacity;
RtVoid RiOpacity( RtColor Os );

%rename(TextureCoordinates) RiTextureCoordinates;
RtVoid RiTextureCoordinates( RtFloat s1, RtFloat t1, RtFloat s2, RtFloat t2, RtFloat s3, RtFloat t3, RtFloat s4, RtFloat t4 );

%rename(LightSource) RiLightSourceV;
RtLightHandle RiLightSourceV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(AreaLightSource) RiAreaLightSourceV;
RtLightHandle RiAreaLightSourceV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Illuminate) RiIlluminate;
RtVoid RiIlluminate( RtLightHandle light, RtBoolean onoff );

%rename(Surface) RiSurfaceV;
RtVoid RiSurfaceV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Deformation) RiDeformationV;
RtVoid RiDeformationV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Displacement) RiDisplacementV;
RtVoid RiDisplacementV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Atmosphere) RiAtmosphereV;
RtVoid RiAtmosphereV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Interior) RiInteriorV;
RtVoid RiInteriorV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Exterior) RiExteriorV;
RtVoid RiExteriorV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(ShaderLayer) RiShaderLayerV;
RtVoid RiShaderLayerV( RtToken type, RtToken name, RtToken layername, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(ConnectShaderLayers) RiConnectShaderLayers;
RtVoid RiConnectShaderLayers( RtToken type, RtToken layer1, RtToken variable1, RtToken layer2, RtToken variable2 );

%rename(ShadingRate) RiShadingRate;
RtVoid RiShadingRate( RtFloat size );

%rename(ShadingInterpolation) RiShadingInterpolation;
RtVoid RiShadingInterpolation( RtToken type );

%rename(Matte) RiMatte;
RtVoid RiMatte( RtBoolean onoff );

%rename(Bound) RiBound;
RtVoid RiBound( RtBound bound );

%rename(Detail) RiDetail;
RtVoid RiDetail( RtBound bound );
%rename(DetailRange) RiDetailRange;
RtVoid RiDetailRange( RtFloat offlow, RtFloat onlow, RtFloat onhigh, RtFloat offhigh );

%rename(GeometricApproximation) RiGeometricApproximation;
RtVoid RiGeometricApproximation( RtToken type, RtFloat value );

%rename(Orientation) RiOrientation;
RtVoid RiOrientation( RtToken orientation );
%rename(ReverseOrientation) RiReverseOrientation;
RtVoid RiReverseOrientation(  );
%rename(Sides) RiSides;
RtVoid RiSides( RtInt nsides );

%rename(Identity) RiIdentity;
RtVoid RiIdentity(  );
%rename(Transform) RiTransform;
RtVoid RiTransform( RtMatrix transform );
%rename(ConcatTransform) RiConcatTransform;
RtVoid RiConcatTransform( RtMatrix transform );
%rename(Perspective) RiPerspective;
RtVoid RiPerspective( RtFloat fov );
%rename(Translate) RiTranslate;
RtVoid RiTranslate( RtFloat dx, RtFloat dy, RtFloat dz );
%rename(Rotate) RiRotate;
RtVoid RiRotate( RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz );
%rename(Scale) RiScale;
RtVoid RiScale( RtFloat sx, RtFloat sy, RtFloat sz );
%rename(Skew) RiSkew;
RtVoid RiSkew( RtFloat angle, RtFloat dx1, RtFloat dy1, RtFloat dz1, RtFloat dx2, RtFloat dy2, RtFloat dz2 );

%rename(CoordinateSystem) RiCoordinateSystem;
RtVoid RiCoordinateSystem( RtToken space );
%rename(CoordSysTransform) RiCoordSysTransform;
RtVoid RiCoordSysTransform( RtToken space );

/* Note: need to process this one differently */
%rename(TransformPoints) RiTransformPoints;
RtPoint* RiTransformPoints( RtToken fromspace, RtToken tospace, RtInt npoints, RtPoint points[] );

%rename(TransformBegin) RiTransformBegin;
RtVoid RiTransformBegin(  );
%rename(TransformEnd) RiTransformEnd;
RtVoid RiTransformEnd(  );

%rename(Resource) RiResourceV;
RtVoid RiResourceV( RtToken handle, RtToken type, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(ResourceBegin) RiResourceBegin;
RtVoid RiResourceBegin(  );
%rename(ResourceEnd) RiResourceEnd;
RtVoid RiResourceEnd(  );

%rename(Attribute) RiAttributeV;
RtVoid RiAttributeV( RtToken name, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Polygon) RiPolygonV;
RtVoid RiPolygonV( RtInt nvertices, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(GeneralPolygon) RiGeneralPolygonV;
RtVoid RiGeneralPolygonV( RtInt nloops, RtInt nverts[], RtInt count, RtToken tokens[], RtPointer values[] );
%rename(PointsPolygons) RiPointsPolygonsV;
RtVoid RiPointsPolygonsV( RtInt npolys, RtInt nverts[], RtInt verts[], RtInt count, RtToken tokens[], RtPointer values[] );
%rename(PointsGeneralPolygons) RiPointsGeneralPolygonsV;
RtVoid RiPointsGeneralPolygonsV( RtInt npolys, RtInt nloops[], RtInt nverts[], RtInt verts[], RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Basis) RiBasis;
RtVoid RiBasis( RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep );
%rename(Patch) RiPatchV;
RtVoid RiPatchV( RtToken type, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(PatchMesh) RiPatchMeshV;
RtVoid RiPatchMeshV( RtToken type, RtInt nu, RtToken uwrap, RtInt nv, RtToken vwrap, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(NuPatch) RiNuPatchV;
RtVoid RiNuPatchV( RtInt nu, RtInt uorder, RtFloat uknot[], RtFloat umin, RtFloat umax, RtInt nv, RtInt vorder, RtFloat vknot[], RtFloat vmin, RtFloat vmax, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(TrimCurve) RiTrimCurve;
RtVoid RiTrimCurve( RtInt nloops, RtInt ncurves[], RtInt order[], RtFloat knot[], RtFloat min[], RtFloat max[], RtInt n[], RtFloat u[], RtFloat v[], RtFloat w[] );

%rename(SubdivisionMesh) RiSubdivisionMeshV;
RtVoid RiSubdivisionMeshV( RtToken scheme, RtInt nfaces, RtInt nvertices[], RtInt vertices[], RtInt ntags, RtToken tags[], RtInt nargs[], RtInt intargs[], RtFloat floatargs[], RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Sphere) RiSphereV;
RtVoid RiSphereV( RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat thetamax, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Cone) RiConeV;
RtVoid RiConeV( RtFloat height, RtFloat radius, RtFloat thetamax, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Cylinder) RiCylinderV;
RtVoid RiCylinderV( RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat thetamax, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Hyperboloid) RiHyperboloidV;
RtVoid RiHyperboloidV( RtPoint point1, RtPoint point2, RtFloat thetamax, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Paraboloid) RiParaboloidV;
RtVoid RiParaboloidV( RtFloat rmax, RtFloat zmin, RtFloat zmax, RtFloat thetamax, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Disk) RiDiskV;
RtVoid RiDiskV( RtFloat height, RtFloat radius, RtFloat thetamax, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Torus) RiTorusV;
RtVoid RiTorusV( RtFloat majorrad, RtFloat minorrad, RtFloat phimin, RtFloat phimax, RtFloat thetamax, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Points) RiPointsV;
RtVoid RiPointsV( RtInt npoints, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(Curves) RiCurvesV;
RtVoid RiCurvesV( RtToken type, RtInt ncurves, RtInt nvertices[], RtToken wrap, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Blobby) RiBlobbyV;
RtVoid RiBlobbyV( RtInt nleaf, RtInt ncode, RtInt code[], RtInt nflt, RtFloat flt[], RtInt nstr, RtToken str[], RtInt count, RtToken tokens[], RtPointer values[] );

%rename(Procedural) RiProcedural;
/*RtVoid RiProcedural( RtPointer data, RtBound bound, RtProcSubdivFunc refineproc, RtProcFreeFunc freeproc );*/

%rename(Geometry) RiGeometryV;
RtVoid RiGeometryV( RtToken type, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(SolidBegin) RiSolidBegin;
RtVoid RiSolidBegin( RtToken type );
%rename(SolidEnd) RiSolidEnd;
RtVoid RiSolidEnd();

%rename(ObjectBegin) RiObjectBegin;
RtObjectHandle RiObjectBegin(  );
%rename(ObjectEnd) RiObjectEnd;
RtVoid RiObjectEnd(  );
%rename(ObjectInstance) RiObjectInstance;
RtVoid RiObjectInstance( RtObjectHandle handle );

%typemap(in) (int, float INPUT[])
%{	
	$2 = SWIG_get_float_num_array_var(L,$input,&$1);
	if (!$2) SWIG_fail;
%}
%apply (int, float INPUT[]) {(RtInt N, RtFloat times[])}
%rename(MotionBegin) RiMotionBeginV;
RtVoid RiMotionBeginV( RtInt N, RtFloat times[] );
%rename(MotionEnd) RiMotionEnd;
RtVoid RiMotionEnd(  );

%rename(MakeTexture) RiMakeTextureV;
RtVoid RiMakeTextureV( RtString imagefile, RtString texturefile, RtToken swrap, RtToken twrap, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(MakeBump) RiMakeBumpV;
RtVoid RiMakeBumpV( RtString imagefile, RtString bumpfile, RtToken swrap, RtToken twrap, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(MakeLatLongEnvironment) RiMakeLatLongEnvironmentV;
RtVoid RiMakeLatLongEnvironmentV( RtString imagefile, RtString reflfile, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(MakeCubeFaceEnvironment) RiMakeCubeFaceEnvironmentV;
RtVoid RiMakeCubeFaceEnvironmentV( RtString px, RtString nx, RtString py, RtString ny, RtString pz, RtString nz, RtString reflfile, RtFloat fov, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, RtInt count, RtToken tokens[], RtPointer values[] );

%rename(MakeShadow) RiMakeShadowV;
RtVoid RiMakeShadowV( RtString picfile, RtString shadowfile, RtInt count, RtToken tokens[], RtPointer values[] );
%rename(MakeOcclusion) RiMakeOcclusionV;
RtVoid RiMakeOcclusionV( RtInt npics, RtString picfiles[], RtString shadowfile, RtInt count, RtToken tokens[], RtPointer values[] );

/*RtVoid RiErrorHandler( RtErrorFunc handler );*/

%rename(ArchiveRecord) RiArchiveRecord;
RtVoid RiArchiveRecord( RtToken type, char * format,  ... );
%rename(ReadArchive) RiReadArchiveV;
RtVoid RiReadArchiveV( RtToken name, RtArchiveCallback callback, RtInt count, RtToken tokens[], RtPointer values[] );
