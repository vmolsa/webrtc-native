
import Stream = require('stream');

import {
  RTCStats,
  RTCStatsReport,
  RTCStatsProvider,
  RTCIceCandidate,
  RTCIceGatherer,
  RTCIceComponent,
  RTCIceRole,
  RTCIceTransportState,
  RTCIceCandidatePair,
  RTCIceParameters,
  RTCIceGatherCandidate,
} from './ortc'

/**
 * The RTCIceTransport allows an application access to information about the Interactive Connectivity Establishment (ICE) 
 * transport over which packets are sent and received. In particular, 
 * ICE manages peer-to-peer connections which involve state which the application may want to access.
 */

export class RTCIceTransport extends Stream.Duplex implements RTCStatsProvider {
    /*
     * onstatechange() => emit('statechange')
     * oncandidatepairchange(event: RTCIceCandidatePairChangedEvent) => emit('candidatepairchange')
     */
  
    /**
     * An RTCIceTransport instance is constructed (optionally) from an RTCIceGatherer. 
     * If gatherer.state is closed or gatherer.component is rtcp, then throw an InvalidStateError exception.
     * 
     * An RTCIceTransport object in the closed state can be garbage-collected when it is no longer referenced.
     */
  
    constructor(gatherer?: RTCIceGatherer) {
      super();
    }
  
    private _iceGatherer: RTCIceGatherer;
    private _role: RTCIceRole;
    private _component: RTCIceComponent;
    private _state: RTCIceTransportState;
  
    /**
     * The iceGatherer attribute is set to the value of gatherer if passed in the constructor or in the latest call to start().
     */
  
    public get iceGatherer(): RTCIceGatherer {
      return this._iceGatherer;
    }
  
    /**
     * The current role of the ICE transport.
     */
  
    public get role(): RTCIceRole {
      return this._role;
    }
  
    /**
     * The component-id of the RTCIceTransport object. In RTCIceTransport objects returned by createAssociatedTransport(), 
     * the value of component is rtcp. In all other RTCIceTransport objects, the value of component is rtp.
     */
  
    public get component(): RTCIceComponent {
      return this._component;
    }
  
    /**
     * The current state of the ICE transport.
     */
  
    public get state(): RTCIceTransportState {
      return this._state;
    }
  
    /**
     * Retrieve the sequence of candidates associated with the remote RTCIceTransport. 
     * Only returns the candidates previously added using setRemoteCandidates() or addRemoteCandidate(). 
     * If there are no remote candidates, an empty list is returned.
     */
  
    public getRemoteCandidates(): RTCIceCandidate[] {
      return undefined;
    }
  
    /**
     * Retrieves the selected candidate pair on which packets are sent. 
     * If there is no selected pair yet, or consent [RFC7675] is lost on the selected pair, NULL is returned.
     */
  
    public getSelectedCandidatePair(): RTCIceCandidatePair {
      return undefined;
    }
  
    /**
     * 
     * As noted in [RFC5245] Section 7.1.2.3, an incoming connectivity check utilizes the local/remote username fragment and the local password, 
     * whereas an outgoing connectivity check utilizes the local/remote username fragment and the remote password. 
     * Since start() provides role information, as well as the remote username fragment and password, 
     * once start() is called an RTCIceTransport object can respond to incoming connectivity checks based on its configured role. 
     * Since start() enables candidate pairs to be formed, it also enables initiating connectivity checks.
     * 
     * When start() is called, the following steps must be run:
     * - If gatherer.component has a value different from component, throw an InvalidParameters exception.
     * - If state or gatherer.state is closed, throw an InvalidStateError exception.
     * - If remoteParameters.usernameFragment or remoteParameters.password is unset, throw an InvalidParameters exception.
     * - If start() is called again and role is changed, throw an InvalidParameters exception.
     * - If start() is called again with the same values of gatherer and remoteParameters, this has no effect.
     * - If start() is called for the first time and either gatherer was not passed in 
     *   the constructor or the value of gatherer is unchanged, if there are remote candidates, 
     *   set state to checking and start connectivity checks. If there are no remote candidates, state remains new.
     * - If start() is called for the first time and the value of gatherer passed as an argument 
     *   is different from that passed in the constructor, flush local candidates. 
     *   If there are remote candidates, set state to checking and start connectivity checks. 
     *   If there are no remote candidates, state remains new.
     * - If start() is called again with the same value of gatherer but the value of remoteParameters has changed, 
     *   local candidates are kept, remote candidates are flushed, candidate pairs are flushed and state transitions to new.
     * - If start() is called again with a new value of gatherer but the value of remoteParameters is unchanged, 
     *   local candidates are flushed, candidate pairs are flushed, new candidate pairs are formed with existing remote candidates, and state transitions to checking.
     * - If start() is called again with new values of gatherer and remoteParameters, local candidates are flushed, 
     *   remote candidates are flushed, candidate pairs are flushed and state transitions to new.
     */
  
    public start(gatherer: RTCIceGatherer, 
                 remoteParameters: RTCIceParameters, 
                 role: RTCIceRole = RTCIceRole.controlled): void
    {
      
    }
    
    /**
     * Irreversibly stops the RTCIceTransport. When stop is called, the following steps must be run:
     * 
     * - Let iceTransport be the RTCIceTransport object on which the stop method is invoked.
     * - If iceTransport.state is closed, abort these steps.
     * - Set iceTransport.state to closed.
     * - Let controller be the RTCIceTransportController object that iceTransport has been added to.
     * - Remove iceTransport from controller.
     * - Fire a simple event statechange at iceTransport.
     */
  
    public stop(): void {
  
    }
  
    /**
     * getRemoteParameters() obtains the current ICE parameters of the remote RTCIceTransport.
     */
  
    public getRemoteParameters(): RTCIceParameters {
      return undefined;
    }
  
    /**
     * Create an associated RTCIceTransport for RTCP. If called more than once for the same component, 
     * or if state is closed, throw an InvalidStateError exception. If called when component is rtcp, 
     * throw an InvalidStateError exception.
     */
  
    public createAssociatedTransport(): RTCIceTransport {
      return undefined;
    } 
  
    /**
     * Add a remote candidate associated with the remote RTCIceTransport. 
     * If state is closed, throw an InvalidStateError exception. When the remote RTCIceGatherer emits its final candidate, 
     * addRemoteCandidate() should be called with an RTCIceCandidateComplete dictionary as an argument, 
     * so that the local RTCIceTransport can know there are no more remote candidates expected, and can enter the completed state.
     */
    
    public addRemoteCandidate(remoteCandidate: RTCIceGatherCandidate): void {
  
    }
  
    /**
     * Set the sequence of candidates associated with the remote RTCIceTransport. If state is closed, throw an InvalidStateError exception.
     */
  
    public setRemoteCandidates(remoteCandidates: RTCIceCandidate[]): void {
  
    }
  
    /**
     * Gathers stats for the given object and reports the result asynchronously. 
     * If the object has not yet begun to send or receive data, 
     * the returned stats will reflect this. 
     * If the object is in the closed state, 
     * the returned stats will reflect the stats at the time the object transitioned to the closed state.
     * 
     * When the getStats method is invoked, the user agent must queue a task to run the following steps:
     * - Let p be a new promise.
     * - Return, but continue the following steps in the background.
     * - Start gathering the stats.
     * - When the relevant stats have been gathered, return a new RTCStatsReport object, 
     *   representing the gathered stats.
     */
  
    public getStats(): Promise<RTCStatsReport> {
      return undefined;
    }
  };