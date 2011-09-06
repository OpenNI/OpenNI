package org.OpenNI;

public class UserGenerator extends Generator
{
	UserGenerator(Context context, long nodeHandle, boolean addRef) throws GeneralException 
	{
		super(context, nodeHandle, addRef);
		
		newUserEvent = new Observable<UserEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterUserCallbacks(toNative(), this, "callback", null, phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterUserCallbacks(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int user)
			{
				notify(new UserEventArgs(user));
			}
		};
		lostUserEvent = new Observable<UserEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterUserCallbacks(toNative(), this, null, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterUserCallbacks(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int user)
			{
				notify(new UserEventArgs(user));
			}
		};
		userExitEvent = new Observable<UserEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToUserExit(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromUserExit(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int user)
			{
				notify(new UserEventArgs(user));
			}
		};
		userReenterEvent = new Observable<UserEventArgs>()
		{
			@Override
			protected int registerNative(OutArg<Long> phCallback) throws StatusException 
			{
				return NativeMethods.xnRegisterToUserReEnter(toNative(), this, "callback", phCallback);
			}

			@Override
			protected void unregisterNative(long hCallback) 
			{
				NativeMethods.xnUnregisterFromUserExit(toNative(), hCallback);
			}
			
			@SuppressWarnings("unused")
			public void callback(int user)
			{
				notify(new UserEventArgs(user));
			}
		};
	}
	
	public static UserGenerator create(Context context, Query query, EnumerationErrors errors) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateUserGenerator(context.toNative(), handle,
			query == null ? 0 : query.toNative(),
			errors == null ? 0 : errors.toNative());
		WrapperUtils.throwOnError(status);
		UserGenerator result = (UserGenerator)context.createProductionNodeObject(handle.value, NodeType.USER);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	public static UserGenerator create(Context context, Query query) throws GeneralException
	{
		return create(context, query, null);
	}

	public static UserGenerator create(Context context) throws GeneralException
	{
		return create(context, null, null);
	}
	
	public int getNumberOfUsers()
	{
		return NativeMethods.xnGetNumberOfUsers(toNative());
	}
	public int[] getUsers() throws StatusException
	{
		OutArg<Integer[]> users = new OutArg<Integer[]>();
		int status = NativeMethods.xnGetUsers(toNative(), users);
		WrapperUtils.throwOnError(status);
		
		int[] outUsers = new int[users.value.length];
		for (int i = 0; i < users.value.length; ++i)
		{
			outUsers[i] = users.value[i];
		}
		
		return outUsers;
	}
	public Point3D getUserCoM(int id) throws StatusException
	{
		OutArg<Point3D> com = new OutArg<Point3D>();
		int status = NativeMethods.xnGetUserCoM(toNative(), id, com);
		WrapperUtils.throwOnError(status);
		return com.value;
	}
	
	public void getUserPixels(int user, SceneMetaData smd)
	{
		NativeMethods.xnGetUserPixels(toNative(), user, smd);
	}
	public SceneMetaData getUserPixels(int user)
	{
		SceneMetaData smd = new SceneMetaData();
		getUserPixels(user, smd);
		return smd;
	}

	public PoseDetectionCapability getPoseDetectionCapability() throws StatusException
	{
		return new PoseDetectionCapability(this);
	}
	public SkeletonCapability getSkeletonCapability() throws StatusException
	{
		return new SkeletonCapability(this);
	}
	
	
	// Events
	public IObservable<UserEventArgs> getNewUserEvent()
	{
		return newUserEvent;
	}
	public IObservable<UserEventArgs> getLostUserEvent()
	{
		return lostUserEvent;
	}
	public IObservable<UserEventArgs> getUserExitEvent()
	{
		return userExitEvent;
	}
	public IObservable<UserEventArgs> getUserReenterEvent()
	{
		return userReenterEvent;
	}
	
	private Observable<UserEventArgs> newUserEvent;
	private Observable<UserEventArgs> lostUserEvent;
	private Observable<UserEventArgs> userExitEvent;
	private Observable<UserEventArgs> userReenterEvent;
	
}
